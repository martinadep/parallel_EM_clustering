#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "include/commons.h"
#include "matrix/matrix_utils.h"

/*
    Multivariate Gaussian Probability of a single data point x (features vector)
*/
T multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix) {

    // (x - mean)
    T* x_mu = malloc(dim * sizeof(T));
    for (int i = 0; i < dim; i++) {
        x_mu[i] = x[i] - means[i];
    }

    // (cov_matrix)^(-1)
    T** inv_cov_matrix = alloc_matrix(dim, dim);
    if (invert_matrix(cov_matrix, dim, inv_cov_matrix) != 0) {
        print_matrix(cov_matrix, dim);
        print_matrix(inv_cov_matrix, dim);
        // singlular matrix, non invertible
        printf("Covariance matrix is singular, cannot compute its inverse.\n");
        free(x_mu);
        free_matrix(inv_cov_matrix, dim);
        return 0.0;
    }

    // (x - mean)^T * (cov_matrix)^(-1)
    T* x_mu_by_inv_cov = malloc(dim * sizeof(T));
    mat_vec_mult(inv_cov_matrix, x_mu, x_mu_by_inv_cov, dim);

    // dotProduct of the above with (x - mean)
    T dot = dot_product(x_mu_by_inv_cov, x_mu, dim);
    free(x_mu);
    free(x_mu_by_inv_cov);

    // e^(-0.5 * dot))
    T exp_res = exp(-0.5 * dot);
    // det(cov_matrix)
    T det = determinant(cov_matrix, dim);
    // normalization constant
    T norm_const =1 / sqrt(pow(2 * PI, dim) * det);
    
    T p = norm_const * exp_res;

    free_matrix(inv_cov_matrix, dim);
    return p;
}

// /*
//     Function that resets the values of the covariance matrix if it becomes the singular.
// */
// void reset_cov(double *cov, int k, int D)
// {
//     int start_ind = k * D * D;

//     for (int r = 0; r < D; r++)
//         for (int c = 0; c < D; c++)
//             if (r == c)
//                 cov[start_ind + r * D + c] = 1.;
//             else
//                 cov[start_ind + r * D + c] = 0.;
// }

// /*
//     Function that resets the values of the mean vector if the covariance matrix becomes the singular.
// */
// void reset_mean(double *mean, int k, int D)
// {
//     int start_ind = k * D;
//     for (int d = 0; d < D; d++)
//         mean[start_ind + d] = (rand() % 10 + 1) * 0.1;
// }

// /*
//     Function that copies the values of mean of covariance
// */
// void get_cluster_mean_cov(double *mean, double *cov, double *m_res, double *cov_res, int k, int D)
// {
//     int start_ind = k * D * D;
//     for (int r = 0; r < D; r++)
//         for (int d = 0; d < D; d++)
//             cov_res[r * D + d] = cov[start_ind + r * D + d];

//     for (int d = 0; d < D; d++)
//         m_res[d] = mean[k * D + d];
// }