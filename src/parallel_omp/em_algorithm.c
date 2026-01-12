#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

// -------------------- E-Step --------------------
void e_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    // Temporary array for class_resp
    T* temp_class_resp = (T*)calloc(num_clusters, sizeof(T));

    for(int k = 0; k < num_clusters; k++) {
        gmm[k].class_resp = 0.0;
    }

    #pragma omp parallel for reduction(+:temp_class_resp[:num_clusters])
    for(int i = 0; i < num_data_points; i++){ 
        T norm = 0.0;
        
        // Calculate PDF and unnormalized responsibility
        for(int k = 0; k < num_clusters; k++){
            T pdf = multiv_gaussian_pdf(data_points[i], dim, gmm[k].mean, gmm[k].cov); 
            resp[i][k] = gmm[k].weight * pdf;
            norm += resp[i][k];
        }

        // Normalization and accumulation in temp_class_resp
        for(int k = 0; k < num_clusters; k++){ 
            resp[i][k] /= norm;
            temp_class_resp[k] += resp[i][k];
        } 
    }

    // Copy the final results into the structure array
    for(int k = 0; k < num_clusters; k++) {
        gmm[k].class_resp = temp_class_resp[k];
    }

    free(temp_class_resp);
}

void m_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    for(int k = 0; k < num_clusters; k++) {
        // 1. Update weights 
        gmm[k].weight = gmm[k].class_resp / num_data_points;

        // 2. Update means
        for (int d = 0; d < dim; d++) {
            T sum_mean = 0.0;
            #pragma omp parallel for reduction(+:sum_mean)
            for (int i = 0; i < num_data_points; i++) {
                sum_mean += resp[i][k] * data_points[i][d];
            }
            gmm[k].mean[d] = sum_mean / gmm[k].class_resp;
        }

        // 3. Update covariance matrices
        for(int i = 0; i < dim; i++) {
            for(int j = 0; j < dim; j++) {
                T sum_cov = 0.0;
                #pragma omp parallel for reduction(+:sum_cov)
                for(int n = 0; n < num_data_points; n++) {
                    T diff_i = data_points[n][i] - gmm[k].mean[i];
                    T diff_j = data_points[n][j] - gmm[k].mean[j];
                    sum_cov += resp[n][k] * diff_i * diff_j;
                }
                gmm[k].cov[i][j] = sum_cov / gmm[k].class_resp;
            }
            gmm[k].cov[i][i] += 1e-6; // Regularization
        }
    }
}

void em_algorithm(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    T** resp = alloc_matrix(num_data_points, num_clusters); // Responsibility matrix
    T prev_log_likelihood = -INFINITY;

    for(int iter = 0; iter < MAX_ITER; iter++){
        // E-step
        e_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        // M-step
        m_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        double log_lik = log_likelihood(data_points, dim, num_data_points, gmm, num_clusters);

        if(fabs(log_lik - prev_log_likelihood) < EPSILON){
            printf("[DEBUG] Convergence reached at iteration %d.\n", iter + 1);
            break;
        }
        prev_log_likelihood = log_lik;
    }

    #pragma omp parallel for
    for (int n = 0; n < num_data_points; n++) {
        int max_k = 0;
        for (int k = 1; k < num_clusters; k++)
            if (resp[n][k] > resp[n][max_k])
                max_k = k;
        labels[n] = max_k;
    }

    free_matrix(resp, num_data_points);
}