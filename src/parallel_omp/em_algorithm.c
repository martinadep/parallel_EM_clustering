#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h> 

#include "../include/matrix_utils.h"
#include "../include/commons.h"

// E-Step: Computes responsibilities for all data points
void e_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    // Reset class responsibilities
    for(int k = 0; k < num_clusters; k++){
        gmm[k].class_resp = 0.0;
    }

    #pragma omp parallel for
    for(int i = 0; i < num_data_points; i++){
        T norm = 0.0;
        for(int k = 0; k < num_clusters; k++){
            T pdf = multiv_gaussian_pdf(data_points[i], dim, gmm[k].mean, gmm[k].cov); 
            resp[i][k] = gmm[k].weight * pdf;
            norm += resp[i][k];
        }

        // Normalize responsibilities
        for(int k = 0; k < num_clusters; k++){
            resp[i][k] /= norm;
        }
    }

    // Accumulate class responsibilities
    for(int k = 0; k < num_clusters; k++){
        T sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
        for(int i = 0; i < num_data_points; i++){
            sum += resp[i][k];
        }
        gmm[k].class_resp = sum;
    }
}
void m_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    // Step 1: Update class responsibilities (sum of resp for each cluster)
    #pragma omp parallel for
    for(int k = 0; k < num_clusters; k++){
        T sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
        for(int n = 0; n < num_data_points; n++){
            sum += resp[n][k];
        }
        gmm[k].class_resp = sum;
        gmm[k].weight = sum / num_data_points;
    }

    // Step 2: Compute weighted means
    #pragma omp parallel for
    for(int k = 0; k < num_clusters; k++){
        T* mean = gmm[k].mean;
        T resp_sum = gmm[k].class_resp;

        // Initialize mean
        for(int d = 0; d < dim; d++) mean[d] = 0.0;

        // Weighted sum
        #pragma omp parallel for reduction(+:mean[:dim])
        for(int n = 0; n < num_data_points; n++){
            for(int d = 0; d < dim; d++){
                mean[d] += resp[n][k] * data_points[n][d];
            }
        }

        // Normalize mean
        for(int d = 0; d < dim; d++){
            mean[d] /= resp_sum;
        }
    }

    // Step 3: Compute weighted covariance matrices
    #pragma omp parallel for
    for(int k = 0; k < num_clusters; k++){
        T** cov = gmm[k].cov;
        T* mean = gmm[k].mean;
        T resp_sum = gmm[k].class_resp;

        // Initialize covariance
        for(int i = 0; i < dim; i++)
            for(int j = 0; j < dim; j++)
                cov[i][j] = 0.0;

        // Compute weighted outer products
        #pragma omp parallel
        {
            // Thread-local private covariance accumulator
            T local_cov[dim][dim];
            for(int i = 0; i < dim; i++)
                for(int j = 0; j < dim; j++)
                    local_cov[i][j] = 0.0;

            #pragma omp for nowait
            for(int n = 0; n < num_data_points; n++){
                T r = resp[n][k];
                for(int i = 0; i < dim; i++){
                    T diff_i = data_points[n][i] - mean[i];
                    for(int j = 0; j < dim; j++){
                        T diff_j = data_points[n][j] - mean[j];
                        local_cov[i][j] += r * diff_i * diff_j;
                    }
                }
            }

            // Combine thread-local covariances into global cov
            #pragma omp critical
            {
                for(int i = 0; i < dim; i++)
                    for(int j = 0; j < dim; j++)
                        cov[i][j] += local_cov[i][j];
            }
        }

        // Normalize and regularize
        for(int i = 0; i < dim; i++){
            for(int j = 0; j < dim; j++){
                cov[i][j] /= resp_sum;
            }
            cov[i][i] += 1e-6; // Diagonal regularization
        }
    }
}

// EM Algorithm
void em_algorithm(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    T** resp = alloc_matrix(num_data_points, num_clusters);
    T prev_log_likelihood = -INFINITY;

    for(int iter = 0; iter < MAX_ITER; iter++){
        e_step(data_points, dim, num_data_points, gmm, num_clusters, resp);
        m_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        double log_lik = log_likelihood(data_points, dim, num_data_points, gmm, num_clusters);

        if(fabs(log_lik - prev_log_likelihood) < EPSILON){
            printf("[DEBUG] Convergence reached at iteration %d.\n", iter + 1);
            break;
        }
        prev_log_likelihood = log_lik;
    }

    // Assign final labels
    #pragma omp parallel for
    for(int n = 0; n < num_data_points; n++){
        int max_k = 0;
        for(int k = 1; k < num_clusters; k++){
            if(resp[n][k] > resp[n][max_k])
                max_k = k;
        }
        labels[n] = max_k;
    }

    free_matrix(resp, num_data_points);
}
