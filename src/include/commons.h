#ifndef __COMMONS_H_
#define __COMMONS_H_

typedef float T; 

// Common utility functions implemented in utils.c
T** alloc_matrix(int dim);
void get_minor(T **A, T **minor, int dim, int p, int q);
void free_matrix(T **matrix, int dim);
void print_matrix(T **matrix, int dim);

#endif