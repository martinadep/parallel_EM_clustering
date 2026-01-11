#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "include/commons.h"
#include "include/matrix_utils.h"

// Multivariate Gaussian Probability of a single data point
T multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix) {
    // (x - mean)
    T* x_mu = malloc(dim * sizeof(T));
    for (int i = 0; i < dim; i++) {
        x_mu[i] = x[i] - means[i];
    }

    // (cov_matrix)^(-1)
    T** inv_cov_matrix = alloc_matrix(dim, dim);
    if (invert_matrix(cov_matrix, dim, inv_cov_matrix) != 0) {
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

    T exp_res = exp(-0.5 * dot); // e^(-0.5 * dot))
    T det = determinant(cov_matrix, dim); // det(cov_matrix)
    T norm_const =1 / sqrt(pow(2 * PI, dim) * det); // normalization constant
    T p = norm_const * exp_res;

    free_matrix(inv_cov_matrix, dim);
    return p;
}