#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"   
#include "../include/commons.h"

/*
   Invert matrix using A^{-1} = adj(A) / det(A)
   Returns 0 on success, -1 if singular
 */
int invert_matrix(T **matrix, int dim, T **matrix_inv) {
    T det = determinant(matrix, dim);

    if (det == 0.0f)
        return -1;     // Singular matrix

    T **adj = alloc_matrix(dim);
    adjoint(matrix, adj, dim);

    // matrix_inv = adj / det
    for (int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
            matrix_inv[i][j] = adj[i][j] / det;

    free_matrix(adj, dim);

    return 0;
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

    T **minor = alloc_matrix(dim - 1);

    for (int col = 0; col < dim; col++) {
        get_minor(matrix, minor, dim, 0, col);
        det += sign * matrix[0][col] * determinant(minor, dim - 1);
        sign = -sign;
    }

    free_matrix(minor, dim - 1);

    return det;
}

/*
   Compute adjoint matrix: adj(matrix) = transpose(cofactor(matrix))
 */
void adjoint(T **matrix, T **adj, int dim) {
    T **cofactor_matrix = alloc_matrix(dim);
    cofactor(matrix, cofactor_matrix, dim);

    // transpose(cofactor)
    for (int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
            adj[j][i] = cofactor_matrix[i][j];

    free_matrix(cofactor_matrix, dim);
}

/*
   Compute cofactor matrix
 */
void cofactor(T **matrix, T **cofactor_matrix, int dim) {
    if (dim == 1) {
        cofactor_matrix[0][0] = 1.0f;
        return;
    }

    T **minor = alloc_matrix(dim - 1);

    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            get_minor(matrix, minor, dim, i, j);

            T sign = ((i + j) % 2 == 0) ? 1.0f : -1.0f;
            cofactor_matrix[i][j] = sign * determinant(minor, dim - 1);
        }
    }

    free_matrix(minor, dim - 1);
}

