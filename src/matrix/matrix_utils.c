#include <stdio.h>
#include <stdlib.h>
#include "matrix_utils.h"
#include "../include/commons.h"

/*
   Allocate a dim1 x dim2 matrix
 */
T** alloc_matrix(int dim1, int dim2) {
    T **M = (T**)malloc(dim1 * sizeof(T*));
    for (int i = 0; i < dim1; i++)
        M[i] = (T*)malloc(dim2 * sizeof(T));
    return M;
}


/*
   Compute determinant recursively (Laplace expansion)
 */
T determinant(T **matrix, int dim) {
    if (dim == 1)
        return matrix[0][0];

    if (dim == 2)
        return matrix[0][0]*matrix[1][1] - matrix[0][1]*matrix[1][0];

    T det = 0.0f;
    int sign = 1;

    T **minor = alloc_matrix(dim - 1, dim - 1);

    for (int col = 0; col < dim; col++) {
        get_minor(matrix, minor, dim, 0, col);
        det += sign * matrix[0][col] * determinant(minor, dim - 1);
        sign = -sign;
    }

    free_matrix(minor, dim - 1);

    return det;
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

void mat_mult(T **matrixA, T **matrixB, T **matrixC, int dim){
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            matrixC[i][j] = 0.0;
            for(int k = 0; k < dim; k++) {
                matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
            }
        }
    }
}

void mat_vec_mult(T **matrix, T *vec, T *result, int dim){
    for(int i = 0; i < dim; i++) {
        result[i] = 0.0;
        for(int j = 0; j < dim; j++) {
            result[i] += matrix[i][j] * vec[j];
        }
    }
}
T dot_product(T *vecA, T *vecB, int dim){
    T result = 0.0;
    for(int i = 0; i < dim; i++) {
        result += vecA[i] * vecB[i];
    }
    return result;
}