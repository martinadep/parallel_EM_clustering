#ifndef __MATRIX_H_
#define __MATRIX_H_
#include "../include/commons.h"

// Common utility functions implemented in matrix_utils.c
T** alloc_matrix(int dim1, int dim2);
void get_minor(T **A, T **minor, int dim, int p, int q);
void free_matrix(T **matrix, int dim);
void print_matrix(T **matrix, int dim);
void mat_mult(T **matrixA, T **matrixB, T **matrixC, int dim);
void mat_vec_mult(T **matrix, T *vec, T *result, int dim);
T dot_product(T *vecA, T *vecB, int dim);

// Matrix inversion and related functions implemented in matrix_inverse.c
int invert_matrix(T **matrix, int dim, T **matrix_inv);
T determinant(T **matrix, int dim);

#endif