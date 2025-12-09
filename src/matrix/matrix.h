#ifndef __MATRIX_H_
#define __MATRIX_H_

typedef float T; 

/* --------------------------------------------------------------
    Matrix inversion and related functions --> implementation in matrix_inverse.c
----------------------------------------------------------------- */
int invert_matrix(T **matrix, int dim, T **matrix_inv);
T determinant(T **matrix, int dim);
void adjoint(T **matrix, T **adj, int dim);
void cofactor(T **matrix, T **cofactor_matrix, int dim);

#endif