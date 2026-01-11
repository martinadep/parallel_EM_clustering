#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "include/commons.h"
#include "include/matrix_utils.h"

// Multivariate Gaussian Probability of a single data point.
// Assumes covariance determinant and inverse are precomputed in g.
T multiv_gaussian_pdf(T* x, int dim, const Gaussian* g) {
    if (g->det <= 0.0) return 0.0;

    T* x_mu = malloc(dim * sizeof(T));
    for (int i = 0; i < dim; i++) {
        x_mu[i] = x[i] - g->mean[i];
    }

    T* x_mu_by_inv_cov = malloc(dim * sizeof(T));
    mat_vec_mult(g->inv_cov, x_mu, x_mu_by_inv_cov, dim);

    T dot = dot_product(x_mu_by_inv_cov, x_mu, dim);
    free(x_mu);
    free(x_mu_by_inv_cov);

    T exp_res = exp(-0.5 * dot);
    T norm_const = 1.0 / sqrt(pow(2 * PI, dim) * g->det);
    return norm_const * exp_res;
}
