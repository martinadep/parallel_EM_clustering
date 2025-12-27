#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "include/commons.h"
#include "matrix/matrix_utils.h"

void init_gmm(Gaussian *gmm, int K, int dim, double **data, int N) {
    for (int k = 0; k < K; k++) {
        gmm[k].mean = (double*)malloc(dim * sizeof(double));
        gmm[k].cov = alloc_matrix(dim, dim);
        gmm[k].weight = 1.0 / K;
        
        // Random initialization
        int idx = rand() % N;
        for (int d = 0; d < dim; d++)
            gmm[k].mean[d] = data[idx][d];
        
        // Initialize covariance as identity
        for (int i = 0; i < dim; i++)
            for (int j = 0; j < dim; j++)
                gmm[k].cov[i][j] = (i == j) ? 1.0 : 0.0;
    }
}

int main() {
    printf("Hello, EM Clustering!\n");

    // int dimm = 200;

    // T **example_matrix = alloc_matrix(dimm, dimm);
    // for(int i = 0; i < dimm; i++)
    //     for(int j = 0; j < dimm; j++)
    //         example_matrix[i][j] = (i == j) ? 2.0f : 1.0f;
   

    // print_matrix(example_matrix, 3);

    // T **inverse_matrix = alloc_matrix(dimm, dimm);

    // clock_t start, end;
    // double elapsed;

    // printf("Inverting matrix of dimension %d x %d\n", dim, dim);
    // start = clock();
    // int r1 = invert_matrix(example_matrix, dim, inverse_matrix);
    // end = clock();
    // elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    // if (r1 == 0) {
    //     printf("Inverse matrix:\n");
    //     print_matrix(inverse_matrix, 3);
    // } else {
    //     printf("Matrix is singular and cannot be inverted.\n");
    // }
    // printf("invert_matrix time: %.6f sec\n", elapsed);

    // printf("\nNow using LU decomposition method:\n");
    // start = clock();
    // int r2 = invert_matrix(example_matrix, dimm, inverse_matrix);
    // end = clock();
    // elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    // if (r2 == 0) {
    //     printf("Inverse matrix (LU):\n");
    //     print_matrix(inverse_matrix, 3);
    // } else {
    //     printf("Matrix is singular and cannot be inverted.\n");
    // }
    // printf("invert_matrix_lu time: %.6f sec\n", elapsed);

    // ------- GMM test -------

    // T means_cluster1[2] = {0.034, 0.146};
    // T means_cluster2[2] = {4.746, 5.038};
    // T means_cluster3[2] = {9.991, 10.24};

    // T** cov_matrix = alloc_matrix(2, 2);
    // cov_matrix[0][0] = 2.0; cov_matrix[0][1] = 0.0;
    // cov_matrix[1][0] = 0.0; cov_matrix[1][1] = 2.0;

    // T x1[2] = {0.0, 0.0};
    // T x2[2] = {4.0, 5.0};
    // T x3[2] = {6.0, 8.0};
    
    // T res_x1_gmm1 = multiv_gaussian_pdf(x1, 2, means_cluster1, cov_matrix);
    // T res_x1_gmm2 = multiv_gaussian_pdf(x1, 2, means_cluster2, cov_matrix);
    // T res_x1_gmm3 = multiv_gaussian_pdf(x1, 2, means_cluster3, cov_matrix);

    // T res_x2_gmm1 = multiv_gaussian_pdf(x2, 2, means_cluster1, cov_matrix);
    // T res_x2_gmm2 = multiv_gaussian_pdf(x2, 2, means_cluster2, cov_matrix);
    // T res_x2_gmm3 = multiv_gaussian_pdf(x2, 2, means_cluster3, cov_matrix);

    // T res_x3_gmm1 = multiv_gaussian_pdf(x3, 2, means_cluster1, cov_matrix);
    // T res_x3_gmm2 = multiv_gaussian_pdf(x3, 2, means_cluster2, cov_matrix);
    // T res_x3_gmm3 = multiv_gaussian_pdf(x3, 2, means_cluster3, cov_matrix);

    // printf("pdf results for x1: %.6e, %.6e, %.6e\n", res_x1_gmm1, res_x1_gmm2, res_x1_gmm3);
    // printf("pdf results for x2: %.6e, %.6e, %.6e\n", res_x2_gmm1, res_x2_gmm2, res_x2_gmm3);
    // printf("pdf results for x3: %.6e, %.6e, %.6e\n", res_x3_gmm1, res_x3_gmm2, res_x3_gmm3);

    // printf("pdf results for x1: %f, %f, %f\n", res_x1_gmm1, res_x1_gmm2, res_x1_gmm3);
    // printf("pdf results for x2: %f, %f, %f\n", res_x2_gmm1, res_x2_gmm2, res_x2_gmm3);
    // printf("pdf results for x3: %f, %f, %f\n", res_x3_gmm1, res_x3_gmm2, res_x3_gmm3);

    // T sum_x1 = res_x1_gmm1 + res_x1_gmm2 + res_x1_gmm3;
    // printf("Normalized responsibilities for x1: %.6f, %.6f, %.6f\n", 
    //        res_x1_gmm1/sum_x1, res_x1_gmm2/sum_x1, res_x1_gmm3/sum_x1);
    // T sum_x2 = res_x2_gmm1 + res_x2_gmm2 + res_x2_gmm3;
    // printf("Normalized responsibilities for x2: %.6f, %.6f, %.6f\n", 
    //        res_x2_gmm1/sum_x2, res_x2_gmm2/sum_x2, res_x2_gmm3/sum_x2);
    // T sum_x3 = res_x3_gmm1 + res_x3_gmm2 + res_x3_gmm3;
    // printf("Normalized responsibilities for x3: %.6f, %.6f, %.6f\n", 
    //        res_x3_gmm1/sum_x3, res_x3_gmm2/sum_x3, res_x3_gmm3/sum_x3);



    // -------------

    srand(time(NULL));
    
    int N = 150;      // Number of data points
    int dim = 2;      // Dimensionality
    int K = 3;        // Number of clusters
    
    // Generate synthetic data (3 Gaussian clusters)
    double **data = alloc_matrix(N, dim);
    double centers[3][2] = {{0, 0}, {5, 5}, {10, 0}};
    
    for (int i = 0; i < N; i++) {
        int cluster = i / (N / 3);
        for (int d = 0; d < dim; d++) {
            double noise = ((double)rand() / RAND_MAX - 0.5) * 2.0;
            data[i][d] = centers[cluster][d] + noise;
        }
    }
    
    // Initialize and run EM
    Gaussian *gmm = (Gaussian*)malloc(K * sizeof(Gaussian));
    int *labels = (int*)malloc(N * sizeof(int));
    
    init_gmm(gmm, K, dim, data, N);
    em_algorithm(data, dim, N, gmm, K, labels);
    
    // Print results
    printf("\nCluster Parameters:\n");
    for (int k = 0; k < K; k++) {
        printf("Cluster %d (weight=%.3f):\n", k, gmm[k].weight);
        printf("  Mean: [%.3f, %.3f]\n", gmm[k].mean[0], gmm[k].mean[1]);
        printf("  Covariance:\n");
        printf("    [%.3f, %.3f]\n", gmm[k].cov[0][0], gmm[k].cov[0][1]);
        printf("    [%.3f, %.3f]\n", gmm[k].cov[1][0], gmm[k].cov[1][1]);
    }
    
    // Cleanup
    for (int k = 0; k < K; k++) {
        free(gmm[k].mean);
        free_matrix(gmm[k].cov, dim);
    }
    free(gmm);
    free(labels);
    free_matrix(data, N);

    return 0;
}