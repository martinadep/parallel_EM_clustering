#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "include/commons.h"

T log_likelihood(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters) {
    T log_lik = 0.0;

    #pragma omp parallel for reduction(+:log_lik) schedule(static)
    for(int n = 0; n < num_data_points; n++){
        T prob = 0.0;
        for(int k = 0; k < num_clusters; k++){
            T pdf = multiv_gaussian_pdf(data_points[n], dim, gmm[k].mean, gmm[k].cov);
            prob += gmm[k].weight * pdf;
        }
        log_lik += log(prob);
    }

    return log_lik;
}