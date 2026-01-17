#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/matrix_utils.h"
#include "include/commons.h"

// E-step
void e_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    // Reset class_resp
    for(int k = 0; k < num_clusters; k++) {
        gmm[k].class_resp = 0.0;
    }

    for(int i = 0; i < num_data_points; i++) { 
        T norm = 0.0;
        int row_offset = i * num_clusters; 
        int data_offset = i * dim;         

        // 1. Calculate unnormalized responsibility
        for(int k = 0; k < num_clusters; k++) {
            // Pass pointer of the i-th point &data_points[data_offset]
            T pdf = multiv_gaussian_pdf(&data_points[data_offset], dim, gmm[k].mean, gmm[k].cov); 
            resp[row_offset + k] = gmm[k].weight * pdf;
            norm += resp[row_offset + k];
        }

        // 2. Normalization and accumulation of class_resp
        for(int k = 0; k < num_clusters; k++) { 
            resp[row_offset + k] /= (norm + 1e-18); // Protection against division by zero
            gmm[k].class_resp += resp[row_offset + k];
        } 
    }
}

// M-step
void m_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    for(int k = 0; k < num_clusters; k++) {
        // Update weights
        gmm[k].weight = gmm[k].class_resp / num_data_points;

        // Update means
        for (int d = 0; d < dim; d++) {
            gmm[k].mean[d] = 0.0;
            for (int i = 0; i < num_data_points; i++) {
                gmm[k].mean[d] += resp[i * num_clusters + k] * data_points[i * dim + d];
            }
            gmm[k].mean[d] /= (gmm[k].class_resp + 1e-18);
        }

        // Update covariance matrices
        for(int i = 0; i < dim; i++) {
            for(int j = 0; j < dim; j++) {
                gmm[k].cov[i][j] = 0.0;
                for(int n = 0; n < num_data_points; n++) {
                    T diff_i = data_points[n * dim + i] - gmm[k].mean[i];
                    T diff_j = data_points[n * dim + j] - gmm[k].mean[j];
                    gmm[k].cov[i][j] += resp[n * num_clusters + k] * diff_i * diff_j;
                }
                gmm[k].cov[i][j] /= (gmm[k].class_resp + 1e-18);
            }
        }

        // Regularization
        for (int i = 0; i < dim; i++)
            gmm[k].cov[i][i] += 1e-6;
    }
}

void em_algorithm(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    T* resp = (T*)malloc(num_data_points * num_clusters * sizeof(T));
    T prev_log_likelihood = -INFINITY;

    for(int iter = 0; iter < MAX_ITER; iter++) {
        e_step(data_points, dim, num_data_points, gmm, num_clusters, resp);
        m_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        double log_lik = log_likelihood(data_points, dim, num_data_points, gmm, num_clusters);

        if(fabs(log_lik - prev_log_likelihood) < EPSILON) {
            printf("[DEBUG] Convergence reached at iteration %d.\n", iter + 1);
            break;
        }
        prev_log_likelihood = log_lik;
    }

    // Assign labels
    for (int n = 0; n < num_data_points; n++) {
        int max_k = 0;
        int offset = n * num_clusters;
        for (int k = 1; k < num_clusters; k++) {
            if (resp[offset + k] > resp[offset + max_k])
                max_k = k;
        }
        labels[n] = max_k;
    }

    free(resp);
}