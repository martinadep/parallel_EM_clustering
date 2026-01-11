#ifndef __TIMING_H__
#define __TIMING_H__
#include <stdio.h>
#include <sys/time.h>
#include <omp.h> // Use OpenMP for timing
// #include <mpi.h> // Use MPI_Wtime for total timing
// TODO: switch to MPI_Wtime for all timing

#ifdef TIMING_BREAKDOWN
// Timer definition
#define TIMER_DEF(label) \
    static double start_##label, end_##label; \
    static double duration_##label = 0.0;

// Timer start
#define TIMER_START(label) \
    start_##label = omp_get_wtime();

// Duration calculation
#define TIMER_ELAPSED(label) \
    end_##label = omp_get_wtime(); \
    duration_##label = end_##label - start_##label;

// Print elapsed time
#define TIMER_PRINT(label) \
    printf(#label " execution time: %f s\n", duration_##label);

// Define the total time spent in the function
#define TIMER_SUM_DEF(label) \
    TIMER_DEF(label) \
    static double total_##label##_time = 0.0;

// Accumulates the total time spent in the function
#define TIMER_SUM(label) \
    TIMER_ELAPSED(label) \
    total_##label##_time += duration_##label;

// Print the total time spent in the function
#define TIMER_SUM_PRINT(label) \
    printf(#label " total execution time: %f s\n", total_##label##_time);

#else
#define TIMER_DEF(label)
#define TIMER_START(label)
#define TIMER_ELAPSED(label)
#define TIMER_PRINT(label)
#define TIMER_SUM_DEF(label)
#define TIMER_SUM(label)
#define TIMER_SUM_PRINT(label)
#endif

#ifdef TOTAL_TIMING

// #define TOTAL_TIMER_START(label) \
//     double start_##label, end_##label; \
//     double duration_##label = 0.0; \
//     start_##label = gettimeofday();

// #define TOTAL_TIMER_STOP(label) \
//     end_##label = gettimeofday(); \
//     duration_##label = end_##label - start_##label; \
//     printf(#label " execution time: %f s\n", duration_##label);

#define TOTAL_TIMER_START(label) \
    struct timeval start_##label, end_##label; \
    double duration_##label = 0.0; \
    gettimeofday(&start_##label, NULL);

#define TOTAL_TIMER_STOP(label) \
    gettimeofday(&end_##label, NULL); \
    duration_##label = (end_##label.tv_sec - start_##label.tv_sec) + \
                       (end_##label.tv_usec - start_##label.tv_usec) / 1e6; \
    // printf("\n*** " #label " execution time: %f s ***\n", duration_##label);

#define GET_DURATION(label) \
    printf("%f", duration_##label);

#endif
#endif // __TIMING_H__