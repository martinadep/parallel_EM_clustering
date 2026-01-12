#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

// -------------------- E-Step --------------------
void e_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    T* temp_class_resp = (T*)calloc(num_clusters, sizeof(T));

    #pragma omp parallel for reduction(+:temp_class_resp[:num_clusters])
    for(int i = 0; i < num_data_points; i++) { 
        T norm = 0.0;
        int row_offset = i * num_clusters;
        int data_offset = i * dim;

        for(int k = 0; k < num_clusters; k++) {
            // Passiamo l'indirizzo dell'inizio del vettore i-esimo
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

void m_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    for(int k = 0; k < num_clusters; k++) {
        gmm[k].weight = gmm[k].class_resp / num_data_points;
        T inv_class_resp = 1.0 / (gmm[k].class_resp + 1e-18);

        // Update means
        #pragma omp parallel for
        for (int d = 0; d < dim; d++) {
            T sum_mean = 0.0;
            for (int i = 0; i < num_data_points; i++) {
                sum_mean += resp[i * num_clusters + k] * data_points[i * dim + d];
            }
            gmm[k].mean[d] = sum_mean * inv_class_resp;
        }

        // Update covariance (Sfruttando la simmetria e array 1D)
        #pragma omp parallel for
        for(int i = 0; i < dim; i++) {
            for(int j = i; j < dim; j++) { // Solo metà superiore
                T sum_cov = 0.0;
                for(int n = 0; n < num_data_points; n++) {
                    T diff_i = data_points[n * dim + i] - gmm[k].mean[i];
                    T diff_j = data_points[n * dim + j] - gmm[k].mean[j];
                    sum_cov += resp[n * num_clusters + k] * diff_i * diff_j;
                }
                T final_val = sum_cov * inv_class_resp;
                gmm[k].cov[i][j] = final_val;
                gmm[k].cov[j][i] = final_val; // Simmetria
            }
            gmm[k].cov[i][i] += 1e-6; // Regularization
        }
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