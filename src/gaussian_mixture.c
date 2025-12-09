#include <stdio.h>
#include <math.h>
#include "include/commons.h"

T multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix) {

    // (x - mean)
    T* diff = malloc(dim * sizeof(T));
    for (int i = 0; i < dim; i++) {
        diff[i] = x[i] - means[i];
    }


    
}
