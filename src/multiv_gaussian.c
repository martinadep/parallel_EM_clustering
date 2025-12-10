#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "include/commons.h"
#include "matrix/matrix.h"

T single_multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix) {

    // (x - mean)
    T* diff = malloc(dim * sizeof(T));
    for (int i = 0; i < dim; i++) {
        diff[i] = x[i] - means[i];
    }

    // (cov_matrix)^(-1)
    T** inv_cov_matrix = alloc_matrix(dim);
    if (invert_matrix(cov_matrix, dim, inv_cov_matrix) != 0) {
        // singlular matrix, non invertible
        free(diff);
        free_matrix(inv_cov_matrix, dim);
        return 0.0;
    }

    // (x - mean)^T * (cov_matrix)^(-1)
    T* diff_by_inv_cov = malloc(dim * sizeof(T));
    mat_vec_mult(inv_cov_matrix, diff, diff_by_inv_cov, dim);

    // dotProduct of the above with (x - mean)
    T dot = dot_product(diff_by_inv_cov, diff, dim);
    free(diff);
    free(diff_by_inv_cov);

    // e^(-0.5 * dot))
    T exp_res = exp(-0.5 * dot);
    // det(cov_matrix)
    T det = determinant(cov_matrix, dim);
    // normalization constant
    T norm_const =1 / sqrt(pow(2 * PI, dim) * det);
    
    T pdf_result = norm_const * exp_res;

    free_matrix(inv_cov_matrix, dim);
    return pdf_result;
}
