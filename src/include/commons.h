#ifndef __COMMONS_H_
#define __COMMONS_H_

typedef double T; 
#define MAX_ITER 300
#define EPSILON 1e-6
#define PI 3.14159265358979323846

typedef struct {
    double *mean;      // Mean vector
    double **cov;      // Covariance matrix
    double **inv_cov;  // Inverse of covariance matrix
    double weight;     // Mixture weight (pi_k)
    double class_resp;
} Gaussian;

T multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix); //, T** inv_cov_matrix);
void em_algorithm(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels);
T log_likelihood(T** data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters);

void init_gmm(Gaussian *gmm, int K, int dim, double **data, int N);
T** load_csv(const char* filename, int* num_rows, int* num_cols);
void write_results_csv(const char *filename, T **data, int *labels, int N, int dim);

#endif