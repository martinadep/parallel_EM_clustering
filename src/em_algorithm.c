#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "include/matrix_utils.h"
#include "include/commons.h"

void e_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {

    for(int k = 0; k < num_clusters; k++){
        gmm[k].class_resp = 0.0;
    }

    for(int i = 0; i < num_data_points; i++){ 
        T norm = 0.0;
        // unnormalized responsibility
        for(int k = 0; k < num_clusters; k++){
            T pdf = multiv_gaussian_pdf(data_points[i], dim, gmm[k].mean, gmm[k].cov);
            resp[i][k] = gmm[k].weight * pdf;
            norm += resp[i][k];
        }

        // normalized responsibility
        for(int k = 0; k < num_clusters; k++){ 
            resp[i][k] /= norm;
        }

        // class responsibility
        for(int k = 0; k < num_clusters; k++){
            gmm[k].class_resp += resp[i][k];
        } 
    }
}


void m_step(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, T** resp) {
    for(int k = 0; k < num_clusters; k++){
        // Update weights
        gmm[k].weight = gmm[k].class_resp / num_data_points;

        // update means
        for (int d = 0; d < dim; d++){
            gmm[k].mean[d] = 0.0;
            for (int i = 0; i < num_data_points; i++){
                gmm[k].mean[d] += resp[i][k] * data_points[i][d];
            }
            gmm[k].mean[d] /= gmm[k].class_resp;

        }

        // Update covariance matrices
        for(int i = 0; i < dim; i++){
            for(int j = 0; j < dim; j++){
                gmm[k].cov[i][j] = 0.0;
                for(int n = 0; n < num_data_points; n++){
                    double diff_i = data_points[n][i] - gmm[k].mean[i];
                    double diff_j = data_points[n][j] - gmm[k].mean[j];
                    gmm[k].cov[i][j] += resp[n][k] * diff_i * diff_j;
                }
                gmm[k].cov[i][j] /= gmm[k].class_resp;
            }
        }

        // Add regularization to avoid singular covariance
        for (int i = 0; i < dim; i++)
            gmm[k].cov[i][i] += 1e-6;
    }
}
void em_algorithm(T** data_points, int dim, int num_data_points,
                  Gaussian* gmm, int num_clusters, int* labels)
{
    T** resp = alloc_matrix(num_data_points, num_clusters);
    T prev_log_likelihood = -INFINITY;
    T log_lik = 0.0;
    
    #pragma omp parallel
    {
        for(int iter = 0; iter < MAX_ITER; iter++){

            /* ====================== E STEP ====================== */

            #pragma omp for schedule(static)
            for(int i = 0; i < num_data_points; i++){
                T norm = 0.0;

                for(int k = 0; k < num_clusters; k++){
                    T pdf = multiv_gaussian_pdf(data_points[i], dim,
                                               gmm[k].mean, gmm[k].cov);
                    resp[i][k] = gmm[k].weight * pdf;
                    norm += resp[i][k];
                }

                for(int k = 0; k < num_clusters; k++)
                    resp[i][k] /= norm;
            }

            /* class_resp */
            #pragma omp for schedule(static)
            for(int k = 0; k < num_clusters; k++){
                T sum = 0.0;
                for(int i = 0; i < num_data_points; i++)
                    sum += resp[i][k];
                gmm[k].class_resp = sum;
            }


            /* ====================== M STEP ====================== */

            /* pesi */
            #pragma omp for schedule(static)
            for(int k = 0; k < num_clusters; k++)
                gmm[k].weight = gmm[k].class_resp / num_data_points;

            /* mean */
            #pragma omp for collapse(2) schedule(static)
            for(int k = 0; k < num_clusters; k++)
            for(int d = 0; d < dim; d++){
                T sum = 0.0;
                for(int i = 0; i < num_data_points; i++)
                    sum += resp[i][k] * data_points[i][d];
                gmm[k].mean[d] = sum / gmm[k].class_resp;
            }

            /* cov */
            #pragma omp for collapse(3) schedule(static)
            for(int k = 0; k < num_clusters; k++)
            for(int i = 0; i < dim; i++)
            for(int j = 0; j < dim; j++){
                T sum = 0.0;
                for(int n = 0; n < num_data_points; n++){
                    T di = data_points[n][i] - gmm[k].mean[i];
                    T dj = data_points[n][j] - gmm[k].mean[j];
                    sum += resp[n][k] * di * dj;
                }
                gmm[k].cov[i][j] = sum / gmm[k].class_resp;
            }

            /* regolarizzazione */
            #pragma omp for collapse(2)
            for(int k = 0; k < num_clusters; k++)
            for(int i = 0; i < dim; i++)
                gmm[k].cov[i][i] += 1e-6;


            /* ================= LOG-LIKELIHOOD ================= */
            #pragma omp for reduction(+:log_lik) schedule(static)
            for(int n = 0; n < num_data_points; n++){
                T prob = 0.0;
                for(int k = 0; k < num_clusters; k++){
                    T pdf = multiv_gaussian_pdf(data_points[n], dim,
                                               gmm[k].mean, gmm[k].cov);
                    prob += gmm[k].weight * pdf;
                }
                log_lik += log(prob);
            }

            #pragma omp single
            {
                if(fabs(log_lik - prev_log_likelihood) < EPSILON)
                    iter = MAX_ITER;

                prev_log_likelihood = log_lik;
            }

            #pragma omp barrier
        }
    }

    /* ===================== LABELING ===================== */

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
