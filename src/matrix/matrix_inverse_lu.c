#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../include/commons.h"   
#include "../include/matrix_utils.h"

/* -------------------------------------------------------------
   LUP decomposition: A = P * L * U
   Returns 0 on success, -1 if singular.
------------------------------------------------------------- */
int lu_decompose(T **A, int dim, int *P) {
    for (int i = 0; i < dim; i++)
        P[i] = i;

    for (int k = 0; k < dim; k++) {

        // Find pivot
        T maxA = 0.0f;
        int pivot = -1;
        for (int i = k; i < dim; i++) {
            T val = fabs(A[i][k]);
            if (val > maxA) {
                maxA = val;
                pivot = i;
            }
        }

        if (maxA < 1e-8f)
            return -1; // singular

        // Swap rows
        if (pivot != k) {
            int tmpP = P[k];
            P[k] = P[pivot];
            P[pivot] = tmpP;

            T *tmp = A[k];
            A[k] = A[pivot];
            A[pivot] = tmp;
        }

        // Perform factorization
        for (int i = k + 1; i < dim; i++) {
            A[i][k] /= A[k][k];
            for (int j = k + 1; j < dim; j++) {
                A[i][j] -= A[i][k] * A[k][j];
            }
        }
    }
    return 0;
}

/* -------------------------------------------------------------
   Solve LUx = b using forward + backward substitution
------------------------------------------------------------- */
void lu_solve(T **LU, int dim, int *P, T *b, T *x) {
    T *y = (T*)malloc(dim * sizeof(T));

    // Forward substitution Ly = Pb
    for (int i = 0; i < dim; i++) {
        y[i] = b[P[i]];
        for (int j = 0; j < i; j++)
            y[i] -= LU[i][j] * y[j];
    }

    // Backward substitution Ux = y
    for (int i = dim - 1; i >= 0; i--) {
        x[i] = y[i];
        for (int j = i + 1; j < dim; j++)
            x[i] -= LU[i][j] * x[j];

        x[i] /= LU[i][i];
    }


    free(y);
}

/* -------------------------------------------------------------
   Compute inverse by solving LU * x = e_i for each i
------------------------------------------------------------- */
int invert_matrix(T **matrix, int dim, T **matrix_inv) {
    // Copy A because LU decomposition destroys it
    T **LU = alloc_matrix(dim, dim);
    for (int i = 0; i < dim; i++)
        for (int j = 0; j < dim; j++)
            LU[i][j] = matrix[i][j];

    int *P = (int*)malloc(dim * sizeof(int));
    if (lu_decompose(LU, dim, P) == -1) { 
        return -1; // singular
    }

    T *e = (T*)malloc(dim * sizeof(T));
    T *x = (T*)malloc(dim * sizeof(T));

    // Solve dim linear systems
    for (int col = 0; col < dim; col++) {
        // e = unit vector for column 'col'
        for (int i = 0; i < dim; i++)
            e[i] = (i == col) ? 1.0f : 0.0f;

        lu_solve(LU, dim, P, e, x);

        // Write solution into inverse matrix
        for (int i = 0; i < dim; i++)
            matrix_inv[i][col] = x[i];
    } 

    free(P);
    free(e);
    free(x);
    free_matrix(LU, dim);
    return 0;
}
