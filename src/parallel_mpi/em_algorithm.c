#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

// E-Step: computes responsibilities for local data points
void e_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    // Reset local responsibilities
    for(int k = 0; k < num_clusters; k++){
        gmm[k].class_resp = 0.0;
    }

    for(int i = 0; i < num_data_points; i++){ 
        T norm = 0.0;
        // calculate unnormalized responsibility
        for(int k = 0; k < num_clusters; k++){
            T pdf = multiv_gaussian_pdf(data_points[i], dim, gmm[k].mean, gmm[k].cov); 
            resp[i][k] = gmm[k].weight * pdf;
            norm += resp[i][k];
        }

        // normalize responsibility
        for(int k = 0; k < num_clusters; k++){ 
            resp[i][k] /= norm;
            gmm[k].class_resp += resp[i][k]; // accumulate local class responsibility
        }
    }
}

// M-Step: updates GMM parameters using distributed data
void m_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp, int total_N) {
    
    // update weights and means
    double *local_sum_resp = (double*)calloc(num_clusters, sizeof(double));
    double *global_sum_resp = (double*)malloc(num_clusters * sizeof(double));
    
    double *local_sum_means = (double*)calloc(num_clusters * dim, sizeof(double));
    double *global_sum_means = (double*)malloc(num_clusters * dim * sizeof(double));

    // calculate local partial sums
    for(int k = 0; k < num_clusters; k++){
        local_sum_resp[k] = gmm[k].class_resp;
        for (int i = 0; i < num_data_points; i++){
            for(int d = 0; d < dim; d++) {
                local_sum_means[k*dim + d] += resp[i][k] * data_points[i][d];
            }
        }
    }

    // global reduction: sum partial results from all processes
    MPI_Allreduce(local_sum_resp, global_sum_resp, num_clusters, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
    MPI_Allreduce(local_sum_means, global_sum_means, num_clusters * dim, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    // update GMM parameters with global values
    for(int k = 0; k < num_clusters; k++){
        gmm[k].class_resp = global_sum_resp[k]; // store for next step
        gmm[k].weight = global_sum_resp[k] / total_N;
        
        for(int d = 0; d < dim; d++) {
            gmm[k].mean[d] = global_sum_means[k*dim + d] / global_sum_resp[k];
        }
    }

    // update covariance matrices (using new means)
    double *local_sum_cov = (double*)calloc(num_clusters * dim * dim, sizeof(double));
    double *global_sum_cov = (double*)malloc(num_clusters * dim * dim * sizeof(double));

    for(int k = 0; k < num_clusters; k++){
        for(int n = 0; n < num_data_points; n++){
            for(int i = 0; i < dim; i++){
                double diff_i = data_points[n][i] - gmm[k].mean[i];
                for(int j = 0; j < dim; j++){
                    double diff_j = data_points[n][j] - gmm[k].mean[j];
                    local_sum_cov[k*dim*dim + i*dim + j] += resp[n][k] * diff_i * diff_j;
                }
            }
        }
    }

    // global reduction for covariances
    MPI_Allreduce(local_sum_cov, global_sum_cov, num_clusters * dim * dim, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    for(int k = 0; k < num_clusters; k++){
        for(int i = 0; i < dim; i++){
            for(int j = 0; j < dim; j++){
                gmm[k].cov[i][j] = global_sum_cov[k*dim*dim + i*dim + j] / gmm[k].class_resp;
            }
            gmm[k].cov[i][i] += 1e-6; // regularization to avoid singular matrix
        }
    }

    // free temporary buffers
    free(local_sum_resp); free(global_sum_resp);
    free(local_sum_means); free(global_sum_means);
    free(local_sum_cov); free(global_sum_cov);
}

void em_algorithm(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    int rank, size, total_N;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // calculate total N (assuming equal distribution as specified in main.c)
    MPI_Allreduce(&num_data_points, &total_N, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    T** resp = alloc_matrix(num_data_points, num_clusters); // local responsibility matrix
    T prev_log_likelihood = -INFINITY;

    for(int iter = 0; iter < MAX_ITER; iter++){

        e_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        m_step(data_points, dim, num_data_points, gmm, num_clusters, resp, total_N);

        // calculate distributed log-likelihood
        double local_log_lik = log_likelihood(data_points, dim, num_data_points, gmm, num_clusters);
        double global_log_lik = 0.0;
        MPI_Allreduce(&local_log_lik, &global_log_lik, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        if(rank == 0) {
            if(fabs(global_log_lik - prev_log_likelihood) < EPSILON){
                printf("[DEBUG] Convergence reached at iteration %d.\n", iter + 1);
            }
        }
        
        // check convergence across all processes
        int stop = 0;
        if(fabs(global_log_lik - prev_log_likelihood) < EPSILON) stop = 1;
        MPI_Bcast(&stop, 1, MPI_INT, 0, MPI_COMM_WORLD);
        
        if(stop) break;
        prev_log_likelihood = global_log_lik;
    }

    // assign final labels (locally)
    for (int n = 0; n < num_data_points; n++) {
        int max_k = 0;
        for (int k = 1; k < num_clusters; k++)
            if (resp[n][k] > resp[n][max_k])
                max_k = k;
        labels[n] = max_k;
    }

    free_matrix(resp, num_data_points);
}
