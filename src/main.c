#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <omp.h>
#include "../include/commons.h"
#include "../include/matrix_utils.h"
#include "../include/utils.h"

#ifdef TOTAL_TIMING
#include "../include/timing/timing.h"
#endif


int main(int argc, char *argv[]) {
    int N, dim, K;
    char dataset_path[256], output_path[256];

    parsing(argc, argv, &K, dataset_path, output_path);

    T* dataset = load_csv(dataset_path, &N, &dim);
    if (!dataset) {
        printf("Failed to load dataset\n");
        return 1;
    }
    // printf("[DEBUG] Loaded dataset: %d points, %d dimensions\n", N, dim);
    // printf("[DEBUG] Looking for clusters: %d\n", K);

    Gaussian *gmm = (Gaussian*)malloc(K * sizeof(Gaussian));
    int *labels = (int*)malloc(N * sizeof(int));
    init_gmm(gmm, K, dim, dataset, N);

    // printf("EM clustering...\n");
    // #pragma omp parallel
    // {
    //     #pragma omp master
    //     {
    //         // printf("Running with %d threads\n", omp_get_num_threads());
    //     }
    // }
    // ********** EM Algorithm Execution ************
    TOTAL_TIMER_START(EM_Algorithm)

    em_algorithm(dataset, dim, N, gmm, K, labels);

    TOTAL_TIMER_STOP(EM_Algorithm)
    // **********************************************

    #pragma omp parallel
    {
        #pragma omp master
        {
            printf("%d, %d, %d, %d, ", N, K, dim, omp_get_num_threads());
            GET_DURATION(EM_Algorithm)
            printf("\n");
        }
    }
    
    // Print and save results
    // printf("\nCluster Parameters:\n");
    // printf("%-7s | %-8s | %s\n", "Cluster", "Weight", "Mean");
    // printf("%s\n", "--------+----------+-----------------------------");
    // for (int k = 0; k < K; k++) {
    //     // printf("%-7d | %-8.3f | [", k, gmm[k].weight);
    //     for (int d = 0; d < dim; d++) {
    //         printf("%.3f%s", gmm[k].mean[d], d < dim-1 ? ", " : "");
    //     }
    //     printf("]\n");
    // }
    // write_results_csv(output_path, dataset, labels, N, dim);
    
    // Cleanup
    #pragma omp parallel for
    for (int k = 0; k < K; k++) {
        free(gmm[k].mean);
        free_matrix(gmm[k].cov, dim);
    }
    free(gmm);
    free(labels);
    free(dataset);

    return 0;
}