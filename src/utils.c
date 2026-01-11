#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "include/matrix_utils.h"
#include "include/utils.h"
#include "include/commons.h"

void parsing(int argc, char *argv[], int *num_clusters, char *dataset_path, char *output_path) {
    *num_clusters = DEFAULT_NUM_CLUSTERS;
    strcpy(dataset_path, DEFAULT_DATASET_PATH);
    strcpy(output_path, DEFAULT_OUTPUT_PATH);
    
    if (argc < 2) {
        printf("\nNo arguments provided. Using default values.\n");
        printf("Usage: ./em_clustering [-d <dataset_path>] [-k <num_clusters>] [-o <output_path>]\n\n");
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            strcpy(dataset_path, argv[++i]);
        } else if (strcmp(argv[i], "-k") == 0 && i + 1 < argc) {
            *num_clusters = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            strcpy(output_path, argv[++i]);
        } else {
            printf("Unknown argument: %s\n", argv[i]);
            printf("Usage: ./%s [-d <dataset_path>] [-k <num_clusters>] [-o <output_path>]\n", argv[0]);
            exit(1);
        }
    }
}

T** load_csv(const char* filename, int* num_rows, int* num_cols) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        printf("Error opening file: %s\n", filename);
        return NULL;
    }

    char line[1024];

    /* ---- Read header: count coordinate columns ---- */
    if (!fgets(line, sizeof(line), file)) {
        fclose(file);
        return NULL;
    }

    *num_cols = 0;
    char* token = strtok(line, ",\n");
    while (token) {
        /* stop if a label column is found */
        if (strcmp(token, "label") == 0)
            break;

        (*num_cols)++;
        token = strtok(NULL, ",\n");
    }

    /* ---- Count data rows ---- */
    *num_rows = 0;
    while (fgets(line, sizeof(line), file)) {
        (*num_rows)++;
    }

    /* ---- Allocate matrix ---- */
    T** data = alloc_matrix(*num_rows, *num_cols);
    if (!data) {
        fclose(file);
        return NULL;
    }

    /* ---- Read data ---- */
    rewind(file);
    fgets(line, sizeof(line), file);  // skip header

    for (int i = 0; i < *num_rows; i++) {
        if (!fgets(line, sizeof(line), file)) break;

        token = strtok(line, ",\n");
        for (int j = 0; j < *num_cols && token; j++) {
            data[i][j] = (T)atof(token);
            token = strtok(NULL, ",\n");
        }
    }

    fclose(file);
    return data;
}

void write_results_csv(const char *filename, T **data, int *labels, int N, int dim) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen");
        return;
    }

    /* ---- Write header ---- */
    for (int d = 0; d < dim; d++) {
        fprintf(fp, "x%d,", d + 1);
    }
    fprintf(fp, "label\n");

    /* ---- Write data + labels ---- */
    for (int i = 0; i < N; i++) {
        for (int d = 0; d < dim; d++) {
            fprintf(fp, "%.6f,", data[i][d]);
        }
        fprintf(fp, "%d\n", labels[i] + 1);
    }
    fclose(fp);
    // printf("\n>>> Results saved to %s\n", filename);
}

void init_gmm(Gaussian *gmm, int K, int dim, T **data, int N) {
    srand(time(NULL));
    
    // First mean: random point
    int idx = rand() % N;
    gmm[0].mean = (T*)malloc(dim * sizeof(T));
    for (int d = 0; d < dim; d++) {
        gmm[0].mean[d] = data[idx][d];
    }
    
    // Subsequent means: pick points far from existing means
    for (int k = 1; k < K; k++) {
        T max_dist = -1.0;
        int best_idx = 0;
        
        for (int n = 0; n < N; n++) {
            T min_dist = INFINITY;
            for (int j = 0; j < k; j++) {
                T dist = 0.0;
                for (int d = 0; d < dim; d++) {
                    T diff = data[n][d] - gmm[j].mean[d];
                    dist += diff * diff;
                }
                if (dist < min_dist) min_dist = dist;
            }
            if (min_dist > max_dist) {
                max_dist = min_dist;
                best_idx = n;
            }
        }
        
        gmm[k].mean = (T*)malloc(dim * sizeof(T));
        for (int d = 0; d < dim; d++) {
            gmm[k].mean[d] = data[best_idx][d];
        }
    }
    
    // Compute initial covariance from data
    T** global_cov = alloc_matrix(dim, dim);
    T* data_mean = (T*)calloc(dim, sizeof(T));
    
    for (int n = 0; n < N; n++) {
        for (int d = 0; d < dim; d++) {
            data_mean[d] += data[n][d];
        }
    }
    for (int d = 0; d < dim; d++) data_mean[d] /= N;
    
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            T cov_ij = 0.0;
            for (int n = 0; n < N; n++) {
                cov_ij += (data[n][i] - data_mean[i]) * (data[n][j] - data_mean[j]);
            }
            global_cov[i][j] = cov_ij / N;
        }
    }
    
    for (int k = 0; k < K; k++) {
        gmm[k].weight = 1.0 / K;
        gmm[k].cov = alloc_matrix(dim, dim);
        gmm[k].class_resp = 0.0;
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                gmm[k].cov[i][j] = global_cov[i][j];
            }
        }
    }
    
    free(data_mean);
    free_matrix(global_cov, dim);
}