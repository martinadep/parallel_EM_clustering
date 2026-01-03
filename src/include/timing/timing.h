#ifndef __TIMING_H__
#define __TIMING_H__

#include <stdio.h>
#include <mpi.h> // Usa MPI per il timing


#ifdef TIMING_BREAKDOWN
#include <omp.h> // Use OpenMP for timing
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

// Macro definitions using MPI_Wtime

#define TOTAL_TIMER_START(label) \
    double start_##label, end_##label; \
    double duration_##label = 0.0; \
    start_##label = MPI_Wtime();

#define TOTAL_TIMER_STOP(label) \
    end_##label = MPI_Wtime(); \
    duration_##label = end_##label - start_##label; \
    int _rank; MPI_Comm_rank(MPI_COMM_WORLD, &_rank); \
    if (_rank == 0) printf("\n*** " #label " execution time: %f s ***\n", duration_##label);

#endif

#endif // __TIMING_H__