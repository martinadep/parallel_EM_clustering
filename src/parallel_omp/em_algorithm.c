#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>

#include "../include/matrix_utils.h"
#include "../include/commons.h"

void e_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp)
{
    T* class_resp_acc = calloc(num_clusters, sizeof(T));

    #pragma omp parallel
    {
        T* local_acc = calloc(num_clusters, sizeof(T));

        #pragma omp for schedule(static)
        for(int i = 0; i < num_data_points; i++){
            T norm = 0.0;
            for(int k = 0; k < num_clusters; k++){
                T pdf = multiv_gaussian_pdf(data_points[i], dim, gmm[k].mean, gmm[k].cov);
                resp[i][k] = gmm[k].weight * pdf;
                norm += resp[i][k];
            }

            for(int k = 0; k < num_clusters; k++){
                resp[i][k] /= norm;
                local_acc[k] += resp[i][k];
            }
        }

        #pragma omp critical
        {
            for(int k = 0; k < num_clusters; k++)
                class_resp_acc[k] += local_acc[k];
        }

        free(local_acc);
    }

    // aggregation
    #pragma omp parallel for schedule(static)
    for(int k = 0; k < num_clusters; k++)
        gmm[k].class_resp = class_resp_acc[k];

    free(class_resp_acc);
}


void m_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp)
{
    // weights
    #pragma omp parallel for
    for(int k = 0; k < num_clusters; k++)
        gmm[k].weight = gmm[k].class_resp / num_data_points;


    //means
    #pragma omp parallel for collapse(2) schedule(static)
    for(int k = 0; k < num_clusters; k++)
    for(int d = 0; d < dim; d++){
        T sum = 0.0;

        for(int i = 0; i < num_data_points; i++)
            sum += resp[i][k] * data_points[i][d];

        gmm[k].mean[d] = sum / gmm[k].class_resp;
    }

    #pragma omp parallel for collapse(3) schedule(static)
    for(int k = 0; k < num_clusters; k++)
    for(int i = 0; i < dim; i++)
    for(int j = 0; j < dim; j++){
        T sum = 0.0;

        for(int n = 0; n < num_data_points; n++){
            T diff_i = data_points[n][i] - gmm[k].mean[i];
            T diff_j = data_points[n][j] - gmm[k].mean[j];
            sum += resp[n][k] * diff_i * diff_j;
        }

        gmm[k].cov[i][j] = sum / gmm[k].class_resp;
    }

    #pragma omp parallel for collapse(2)
    for(int k = 0; k < num_clusters; k++)
    for(int i = 0; i < dim; i++)
        gmm[k].cov[i][i] += 1e-6;
}

void em_algorithm(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels)
{
    T** resp = alloc_matrix(num_data_points, num_clusters);
    T prev_log_likelihood = -INFINITY;

    for(int iter = 0; iter < MAX_ITER; iter++){

        e_step(data_points, dim, num_data_points, gmm, num_clusters, resp);
        m_step(data_points, dim, num_data_points, gmm, num_clusters, resp);

        double log_lik = log_likelihood(data_points, dim, num_data_points, gmm, num_clusters);

        if(fabs(log_lik - prev_log_likelihood) < EPSILON)
            break;

        prev_log_likelihood = log_lik;
    }

    #pragma omp parallel for schedule(static)
    for (int n = 0; n < num_data_points; n++) {
        int max_k = 0;
        for (int k = 1; k < num_clusters; k++)
            if (resp[n][k] > resp[n][max_k])
                max_k = k;
        labels[n] = max_k;
    }

    free_matrix(resp, num_data_points);
}
