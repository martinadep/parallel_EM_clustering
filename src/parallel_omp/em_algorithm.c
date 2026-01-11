#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

// -------------------- E-Step --------------------
void e_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    // Reset class responsibilities
    for(int k = 0; k < num_clusters; k++){
        gmm[k].class_resp = 0.0;
    }

    // Compute responsibilities in parallel over data points
    #pragma omp parallel for
    for(int n = 0; n < num_data_points; n++){
        T norm = 0.0;
        for(int k = 0; k < num_clusters; k++){
            T pdf = multiv_gaussian_pdf(data_points[n], dim, gmm[k].mean, gmm[k].cov);
            resp[n][k] = gmm[k].weight * pdf;
            norm += resp[n][k];
        }
        for(int k = 0; k < num_clusters; k++){
            resp[n][k] /= norm;
        }
    }

    // Accumulate class responsibilities (sequential over K, parallel over N)
    for(int k = 0; k < num_clusters; k++){
        T sum = 0.0;
        #pragma omp parallel for reduction(+:sum)
        for(int n = 0; n < num_data_points; n++){
            sum += resp[n][k];
        }
        gmm[k].class_resp = sum;
    }
}

// -------------------- M-Step --------------------
void m_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    // Update weights and means
    for(int k = 0; k < num_clusters; k++){
        T resp_sum = gmm[k].class_resp;
        gmm[k].weight = resp_sum / num_data_points;

        // Initialize mean
        T* mean = gmm[k].mean;
        for(int d = 0; d < dim; d++) mean[d] = 0.0;

        // Weighted sum of data points
        #pragma omp parallel for
        for(int n = 0; n < num_data_points; n++){
            for(int d = 0; d < dim; d++){
                #pragma omp atomic
                mean[d] += resp[n][k] * data_points[n][d];
            }
        }

        // Normalize mean
        for(int d = 0; d < dim; d++){
            mean[d] /= resp_sum;
        }
    }

    // Update covariance matrices
    for(int k = 0; k < num_clusters; k++){
        T** cov = gmm[k].cov;
        T* mean = gmm[k].mean;
        T resp_sum = gmm[k].class_resp;

        // Initialize covariance
        for(int i = 0; i < dim; i++)
            for(int j = 0; j < dim; j++)
                cov[i][j] = 0.0;

        // Accumulate weighted outer products
        #pragma omp parallel for
        for(int n = 0; n < num_data_points; n++){
            T r = resp[n][k];
            for(int i = 0; i < dim; i++){
                T diff_i = data_points[n][i] - mean[i];
                for(int j = 0; j < dim; j++){
                    T diff_j = data_points[n][j] - mean[j];
                    #pragma omp atomic
                    cov[i][j] += r * diff_i * diff_j;
                }
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

// -------------------- EM Algorithm --------------------
void em_algorithm(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    T** resp = alloc_matrix(num_data_points, num_clusters);
    T prev_log_likelihood = -INFINITY;

    for(int iter = 0; iter < MAX_ITER; iter++){
        e_step(data_points, dim, num_data_points, gmm, num_clusters, resp);
        m_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        // Compute log-likelihood in parallel
        double log_lik = 0.0;
        #pragma omp parallel for reduction(+:log_lik)
        for(int n = 0; n < num_data_points; n++){
            T p = 0.0;
            for(int k = 0; k < num_clusters; k++){
                p += gmm[k].weight * multiv_gaussian_pdf(data_points[n], dim, gmm[k].mean, gmm[k].cov);
            }
            log_lik += log(p);
        }

        // Check convergence
        if(fabs(log_lik - prev_log_likelihood) < EPSILON){
            printf("[DEBUG] Convergence reached at iteration %d.\n", iter + 1);
            break;
        }
        prev_log_likelihood = log_lik;
    }

    // Assign labels (parallel over N)
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
