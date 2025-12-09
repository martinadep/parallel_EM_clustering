#include <stdio.h>
#include "include/commons.h"
#include <time.h>
#include "matrix/matrix.h"

int main() {
    printf("Hello, EM Clustering!\n");

    int dim = 200;

    float **example_matrix = alloc_matrix(dim);
    for(int i = 0; i < dim; i++)
        for(int j = 0; j < dim; j++)
            example_matrix[i][j] = (i == j) ? 2.0f : 1.0f;
   

    print_matrix(example_matrix, 3);

    float **inverse_matrix = alloc_matrix(dim);

    clock_t start, end;
    double elapsed;

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

    printf("\nNow using LU decomposition method:\n");
    start = clock();
    int r2 = invert_matrix(example_matrix, dim, inverse_matrix);
    end = clock();
    elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    if (r2 == 0) {
        printf("Inverse matrix (LU):\n");
        print_matrix(inverse_matrix, 3);
    } else {
        printf("Matrix is singular and cannot be inverted.\n");
    }
    printf("invert_matrix_lu time: %.6f sec\n", elapsed);

    return 0;
}