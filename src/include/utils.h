#ifndef __UTILS_H_
#define __UTILS_H_
#include "commons.h"

void parsing(int argc, char *argv[], int *num_clusters, char *dataset_path, char *output_path);
T** load_csv(const char* filename, int* num_rows, int* num_cols);
void write_results_csv(const char *filename, T **data, int *labels, int N, int dim);
void init_gmm(Gaussian *gmm, int K, int dim, double **data, int N);

#endif