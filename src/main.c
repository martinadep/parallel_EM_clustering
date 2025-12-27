#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "include/commons.h"
#include "matrix/matrix_utils.h"

int main() {
    int N, dim, K = 3;
    T** dataset = load_csv("../datasets/gmm_data.csv", &N, &dim);
    if (!dataset) {
        printf("Failed to load dataset\n");
        return 1;
    }
    printf("[DEBUG] Loaded dataset: %d points, %d coordinates\n", N, dim);
    
    Gaussian *gmm = (Gaussian*)malloc(K * sizeof(Gaussian));
    int *labels = (int*)malloc(N * sizeof(int));
    init_gmm(gmm, K, dim, dataset, N);

    // ********** EM Algorithm Execution ************
    
    em_algorithm(dataset, dim, N, gmm, K, labels);

    // **********************************************
    
    // Print and save results
    printf("\nCluster Parameters:\n");
    for (int k = 0; k < K; k++) {
        printf("Cluster %d (weight=%.3f):\n", k, gmm[k].weight);
        printf("  Mean: [");
        for (int d = 0; d < dim; d++) {
            printf("%.3f%s", gmm[k].mean[d], d < dim-1 ? ", " : "");
        }
        printf("]\n");
    }
    write_results_csv("../results/em_results.csv", dataset, labels, N, dim);
    
    // Cleanup
    for (int k = 0; k < K; k++) {
        free(gmm[k].mean);
        free_matrix(gmm[k].cov, dim);
    }
    free(gmm);
    free(labels);
    free_matrix(dataset, N);

    return 0;
}