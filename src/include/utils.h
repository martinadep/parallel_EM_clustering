#ifndef __UTILS_H_
#define __UTILS_H_
#include "commons.h"

void init_gmm(Gaussian *gmm, int K, int dim, double **data, int N);
T** load_csv(const char* filename, int* num_rows, int* num_cols);
void write_results_csv(const char *filename, T **data, int *labels, int N, int dim);

#endif