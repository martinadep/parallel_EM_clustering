#include <stdio.h>
#include <stdlib.h>
#include "commons.h"

/*
   Allocate a dim x dim matrix
 */
T** alloc_matrix(int dim) {
    T **M = (T**)malloc(dim * sizeof(T*));
    for (int i = 0; i < dim; i++)
        M[i] = (T*)malloc(dim * sizeof(T));
    return M;
}

/*
   Build a minor (submatrix) by removing row p and col q
*/
void get_minor(T **A, T **minor, int dim, int p, int q) {
    int r = 0, c = 0;
    for (int i = 0; i < dim; i++) {
        if (i == p) continue;
        c = 0;
        for (int j = 0; j < dim; j++) {
            if (j == q) continue;
            minor[r][c++] = A[i][j];
        }
        r++;
    }
}

void free_matrix(T **matrix, int dim) {
    for (int i = 0; i < dim; i++)
        free(matrix[i]);
    free(matrix);
}

void print_matrix(T **matrix, int dim) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            printf("%f ", matrix[i][j]);
        }
        printf("\n");
    }
}