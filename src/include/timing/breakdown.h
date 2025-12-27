#ifndef __BREAKDOWN_H__
#define __BREAKDOWN_H__
// TODO: rewrite using timing.h macros and adapt to EM clustering

#include <float.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

extern unsigned int *point_iterations;
extern unsigned int total_iterations;
extern unsigned int min_iterations;
extern unsigned int max_iterations;
extern double sum_iterations;
extern double sum_sq_iterations;
extern clock_t timing_start, timing_end;
extern double elapsed_seconds;

#define BREAKDOWN_INIT(size) \
    do { \
        point_iterations = (unsigned int*)calloc(size, sizeof(unsigned int)); \
        total_iterations = 0; \
        min_iterations = UINT_MAX; \
        max_iterations = 0; \
        sum_iterations = 0.0; \
        sum_sq_iterations = 0.0; \
    } while(0)

#define BREAKDOWN_RECORD(idx, iters) \
    do { \
        point_iterations[idx] = iters; \
        total_iterations += iters; \
        if (iters < min_iterations) min_iterations = iters; \
        if (iters > max_iterations) max_iterations = iters; \
        sum_iterations += iters; \
        sum_sq_iterations += (iters)*(iters); \
    } while(0)

#define BREAKDOWN_START_TIMER() timing_start = clock()
#define BREAKDOWN_STOP_TIMER()  do { timing_end = clock(); elapsed_seconds = (double)(timing_end - timing_start) / CLOCKS_PER_SEC; } while(0)

#define BREAKDOWN_WRITE_TO_FILE(filename, n_points, bandwidth, cluster_epsilon, epsilon, dtype) \
    do { \
        FILE *f = fopen(filename, "a"); \
        if (f) { \
            double mean = sum_iterations / n_points; \
            double stddev = sqrt(sum_sq_iterations / n_points - mean * mean); \
            fprintf(f, "Bandwidth: %f\n", bandwidth); \
            fprintf(f, "Cluster Epsilon: %f\n", (float)cluster_epsilon); \
            fprintf(f, "Iteration Epsilon: %f\n", (float)epsilon); \
            fprintf(f, "DataType: %s\n", dtype); \
            fprintf(f, "Total Points: %u\n", n_points); \
            fprintf(f, "Total Iterations: %u\n", total_iterations); \
            fprintf(f, "Iterations/sec: %.2f iter/sec\n", total_iterations / elapsed_seconds); \
            fprintf(f, "Elapsed Time: %.2f sec\n", elapsed_seconds); \
            fprintf(f, "Min Iterations: %u\n", min_iterations); \
            fprintf(f, "Max Iterations: %u\n", max_iterations); \
            fprintf(f, "Mean Iterations: %.2f\n", mean); \
            fprintf(f, "Stddev Iterations: %.2f\n", stddev); \
            printf( "Bandwidth: %f\n", bandwidth); \
            printf( "Cluster Epsilon: %f\n", (float)cluster_epsilon); \
            printf( "Iteration Epsilon: %f\n", (float)epsilon); \
            printf( "DataType: %s\n", dtype); \
            printf( "Iterations/sec: %.2f iter/sec\n", total_iterations / elapsed_seconds); \
            printf( "Min Iterations: %u\n", min_iterations); \
            printf( "Max Iterations: %u\n", max_iterations); \
            printf( "Mean Iterations: %.2f\n", mean); \
            fprintf(f, "Iterations per point: "); \
            for (unsigned int i = 0; i < n_points; ++i) fprintf(f, "%u ", point_iterations[i]); \
            fprintf(f, "\n---------------------------------------\n"); \
            fclose(f); \
            printf(">>>> Breakdown written to %s\n", filename); \
        } \
        else { \
            fprintf(stderr, "Error opening file %s\n", filename); \
        } \
    } while(0)

#endif // __BREAKDOWN_H__