#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

void e_step(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T* resp) {
    T* temp_class_resp = (T*)calloc(num_clusters, sizeof(T));

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        T* local_resp = &thread_class_resp[tid * num_clusters];

        // init locale
        for(int k = 0; k < num_clusters; k++)
            local_resp[k] = 0.0;

        #pragma omp for schedule(static)
        for(int i = 0; i < num_data_points; i++) {
            T norm = 0.0;
            int row_offset = i * num_clusters;
            int data_offset = i * dim;

        for(int k = 0; k < num_clusters; k++) {
            T pdf = multiv_gaussian_pdf(&data_points[data_offset], dim, gmm[k].mean, gmm[k].cov); 
            resp[row_offset + k] = gmm[k].weight * pdf;
            norm += resp[row_offset + k];
        }

            for(int k = 0; k < num_clusters; k++) {
                T r = resp[row_offset + k] * inv_norm;
                resp[row_offset + k] = r;
                local_resp[k] += r;
            }
        }
    }

    // riduzione finale
    for(int k = 0; k < num_clusters; k++) {
        T sum = 0.0;
        for(int t = 0; t < nthreads; t++)
            sum += thread_class_resp[t * num_clusters + k];

        gmm[k].class_resp = sum;
    }

    free(thread_class_resp);
}

void m_step(T* data_points, int dim, int num_data_points,
            Gaussian* gmm, int num_clusters, T* resp)
{
    int nthreads = omp_get_max_threads();

    // buffer privati
    T* thread_mean = (T*) malloc(nthreads * dim * sizeof(T));
    T* thread_cov  = (T*) malloc(nthreads * dim * dim * sizeof(T));

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

        for (int d = 0; d < dim; d++) current_mean[d] *= inv_class_resp;

        T* temp_cov = (T*)calloc(dim * dim, sizeof(T));

        #pragma omp parallel for reduction(+:temp_cov[:dim*dim])
        for(int n = 0; n < num_data_points; n++) {
            T r = resp[n * num_clusters + k];
            int n_offset = n * dim;
            for(int i = 0; i < dim; i++) {
                T diff_i = data_points[n_offset + i] - current_mean[i];
                for(int j = i; j < dim; j++) {
                    temp_cov[i * dim + j] += r * diff_i * (data_points[n_offset + j] - current_mean[j]);
                }
            }
        }

        for(int i = 0; i < dim; i++) {
            for(int j = i; j < dim; j++) {
                T sum = 0.0;
                for(int t = 0; t < nthreads; t++)
                    sum += thread_cov[t * dim * dim + i * dim + j];

                T val = sum * inv_class_resp;
                gmm[k].cov[i][j] = val;
                gmm[k].cov[j][i] = val;
                gmm[k].cov[j][i] = val;
            }
            gmm[k].cov[i][i] += 1e-6;
        }
    }

    free(thread_mean);
    free(thread_cov);
}


void em_algorithm(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels) {
    T* resp = (T*)malloc(num_data_points * num_clusters * sizeof(T));
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