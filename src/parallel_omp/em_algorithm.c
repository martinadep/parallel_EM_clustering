#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

// E-step
void e_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    T* temp_class_resp = (T*)calloc(num_clusters, sizeof(T));

    #pragma omp parallel for reduction(+:temp_class_resp[:num_clusters])
    for(int i = 0; i < num_data_points; i++) { 
        T norm = 0.0;
        int row_offset = i * num_clusters;
        int data_offset = i * dim;

        for(int k = 0; k < num_clusters; k++) {
            // Pass pointer to the start of the i-th vector
            T pdf = multiv_gaussian_pdf(&data_points[data_offset], dim, gmm[k].mean, gmm[k].cov); 
            resp[row_offset + k] = gmm[k].weight * pdf;
            norm += resp[row_offset + k];
        }

        for(int k = 0; k < num_clusters; k++) { 
            resp[row_offset + k] /= (norm + 1e-18);
            temp_class_resp[k] += resp[row_offset + k];
        } 
    }

    for(int k = 0; k < num_clusters; k++) {
        gmm[k].class_resp = temp_class_resp[k];
    }
    free(temp_class_resp);
}


// M-step
void m_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    for(int k = 0; k < num_clusters; k++) {
        gmm[k].weight = gmm[k].class_resp / num_data_points;
        T inv_class_resp = 1.0 / (gmm[k].class_resp + 1e-18);
        T* current_mean = gmm[k].mean;
        
        for(int d = 0; d < dim; d++) current_mean[d] = 0.0;

        #pragma omp parallel for reduction(+:current_mean[:dim])
        for (int i = 0; i < num_data_points; i++) {
            T weight = resp[i * num_clusters + k];
            int data_offset = i * dim;
            for (int d = 0; d < dim; d++) {
                current_mean[d] += weight * data_points[data_offset + d];
            }
        }

        // Final normalization
        for (int d = 0; d < dim; d++) current_mean[d] *= inv_class_resp;

        // Update covariance matrix
        T* temp_cov = (T*)calloc(dim * dim, sizeof(T));

        #pragma omp parallel for reduction(+:temp_cov[:dim*dim])
        for(int n = 0; n < num_data_points; n++) {
            T r = resp[n * num_clusters + k];
            int n_offset = n * dim;
            for(int i = 0; i < dim; i++) {
                T diff_i = data_points[n_offset + i] - current_mean[i];
                for(int j = i; j < dim; j++) {
                    // Compute only the upper triangular part
                    temp_cov[i * dim + j] += r * diff_i * (data_points[n_offset + j] - current_mean[j]);
                }
            }
        }

        // Final copy to GMM matrix and regularization
        for(int i = 0; i < dim; i++) {
            for(int j = i; j < dim; j++) {
                T val = temp_cov[i * dim + j] * inv_class_resp;
                gmm[k].cov[i][j] = val;
                gmm[k].cov[j][i] = val; // Symmetry
            }
            gmm[k].cov[i][i] += 1e-6;
        }
        free(temp_cov);
    }
}
void em_algorithm(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    T* resp = (T*)malloc(num_data_points * num_clusters * sizeof(T));
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
        int offset = n * num_clusters;
        for (int k = 1; k < num_clusters; k++) {
            if (resp[offset + k] > resp[offset + max_k])
                max_k = k;
        }
        labels[n] = max_k;
    }

    free(resp);
}