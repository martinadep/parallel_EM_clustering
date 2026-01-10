#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> // MPI Include
#include "include/commons.h"
#include "include/matrix_utils.h"
#include "include/utils.h"

#ifdef TOTAL_TIMING
#include "include/timing/timing.h"
#endif

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv); // Initialize MPI environment

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int N, dim, K;
    char dataset_path[256], output_path[256];
    T** dataset = NULL;
    T* flat_dataset = NULL; // Contiguous buffer for sending data

    // Master process reads the dataset
    if (rank == 0) {
        parsing(argc, argv, &K, dataset_path, output_path);
        dataset = load_csv(dataset_path, &N, &dim);
        if (!dataset) {
            MPI_Abort(MPI_COMM_WORLD, 1);
            return 1;
        }
        printf("[MPI Master] Loaded dataset: %d points, %d coordinates\n", N, dim);

        // Flatten the dataset for MPI_Scatter
        flat_dataset = (T*)malloc(N * dim * sizeof(T));
        for(int i=0; i<N; i++) {
            for(int j=0; j<dim; j++) {
                flat_dataset[i*dim + j] = dataset[i][j];
            }
        }
    }

    // Broadcast problem dimensions to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&dim, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate local data size
    // Note: Assuming N is divisible by size for simplicity.
    // In production, use MPI_Scatterv to handle remainders.
    int local_N = N / size; 
    T* local_flat_data = (T*)malloc(local_N * dim * sizeof(T));

    // Distribute data chunks to all processes
    MPI_Scatter(flat_dataset, local_N * dim, MPI_DOUBLE, 
                local_flat_data, local_N * dim, MPI_DOUBLE, 
                0, MPI_COMM_WORLD);

    // Reconstruct local 2D array structure (array of pointers)
    T** local_dataset = (T**)malloc(local_N * sizeof(T*));
    for(int i=0; i<local_N; i++) {
        local_dataset[i] = &local_flat_data[i * dim];
    }

    // Setup GMM structures
    Gaussian *gmm = (Gaussian*)malloc(K * sizeof(Gaussian));
    int *local_labels = (int*)malloc(local_N * sizeof(int));
    
    // Master initializes GMM parameters, others allocate memory
    if (rank == 0) {
        init_gmm(gmm, K, dim, dataset, N);
    } else {
        for(int k=0; k<K; k++) {
            gmm[k].mean = (double*)malloc(dim * sizeof(double));
            gmm[k].cov = alloc_matrix(dim, dim);
        }
    }

    // Broadcast initial GMM parameters to all processes
    for(int k=0; k<K; k++) {
        MPI_Bcast(gmm[k].mean, dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&gmm[k].weight, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        // Serialize covariance matrix for broadcasting
        T* flat_cov = (T*)malloc(dim*dim*sizeof(T));
        if(rank == 0) {
            for(int i=0; i<dim; i++)
                for(int j=0; j<dim; j++)
                    flat_cov[i*dim+j] = gmm[k].cov[i][j];
        }
        MPI_Bcast(flat_cov, dim*dim, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        if(rank != 0) {
            for(int i=0; i<dim; i++)
                for(int j=0; j<dim; j++)
                    gmm[k].cov[i][j] = flat_cov[i*dim+j];
        }
        free(flat_cov);
    }

    // *** FIX: Allocate and Compute Initial Inverse Covariance on Workers ***
    if (rank != 0) {
        for(int k=0; k<K; k++) {
            gmm[k].inv_cov = alloc_matrix(dim, dim); // Ensure allocation
            // Compute inverse immediately so E-step doesn't crash
            inverse_matrix(gmm[k].cov, gmm[k].inv_cov, dim); 
        }
    } else {
        // Ensure master also has inv_cov ready if it wasn't done in init_gmm
        for(int k=0; k<K; k++) {
             if (gmm[k].inv_cov == NULL) gmm[k].inv_cov = alloc_matrix(dim, dim);
             inverse_matrix(gmm[k].cov, gmm[k].inv_cov, dim);
        }
    }
    // **********************************************************************

    // ********** EM Algorithm Execution ************
    TOTAL_TIMER_START(EM_Algorithm)

    // Run EM algorithm on local data chunk
    em_algorithm(local_dataset, dim, local_N, gmm, K, local_labels);

    TOTAL_TIMER_STOP(EM_Algorithm)
    // **********************************************
    
    // Collect results (labels) from all processes to Master
    int *all_labels = NULL;
    if (rank == 0) {
        all_labels = (int*)malloc(N * sizeof(int));
    }
    MPI_Gather(local_labels, local_N, MPI_INT, 
               all_labels, local_N, MPI_INT, 
               0, MPI_COMM_WORLD);

    // Master prints and saves results
    if (rank == 0) {
        printf("\nCluster Parameters:\n");
        printf("%-7s | %-8s | %s\n", "Cluster", "Weight", "Mean");
        printf("%s\n", "--------+----------+-----------------------------");
        for (int k = 0; k < K; k++) {
            printf("%-7d | %-8.3f | [", k, gmm[k].weight);
            for (int d = 0; d < dim; d++) {
                printf("%.3f%s", gmm[k].mean[d], d < dim-1 ? ", " : "");
            }
            printf("]\n");
        }
        write_results_csv(output_path, dataset, all_labels, N, dim);
        
        free(all_labels);
        free(flat_dataset);
        free_matrix(dataset, N);
    }

    // Cleanup local memory
    free(local_flat_data);
    free(local_dataset); // Frees the array of pointers, data is in local_flat_data
    free(local_labels);
    
    // Free GMM memory
    for (int k = 0; k < K; k++) {
        free(gmm[k].mean);
        free_matrix(gmm[k].cov, dim);
    }
    free(gmm);

    MPI_Finalize();
    return 0;
}