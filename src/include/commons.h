#ifndef __COMMONS_H_
#define __COMMONS_H_

#define MAX_ITER 200
#define EPSILON 1e-6
#define PI 3.14159265358979323846

#define DEFAULT_DATASET_PATH "./datasets/gmm_P10000_K3_D2.csv"
#define DEFAULT_OUTPUT_PATH "./results/em_P10000_K3_D2.csv"
#define DEFAULT_NUM_CLUSTERS 3

typedef double T; 

typedef struct {
    double *mean;      // Mean vector
    double **cov;      // Covariance matrix
    double weight;     // Mixture weight (pi_k)
    double class_resp; // Class responsibility
} Gaussian;

T multiv_gaussian_pdf(T* x, int dim, T* means, T** cov_matrix);
void em_algorithm(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters, int* labels);
T log_likelihood(T* data_points, int dim, int num_data_points, Gaussian* gmm, int num_clusters);

#endif