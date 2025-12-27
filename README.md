# EM clustering parallelization
## Table of Contents

1. [Introduction](#introduction)
2. [Prerequisites](#prerequisites)
3. [Quick Start](#quick-start)
4. [Useful Scripts](#useful-scripts)
5. [Performance](#performance)


## Introduction

This project is for the High Performance Computing for Data Science course, offered by University of Trento in 2025. 
This involves the **parallelization of the Expectation Maximization algorithm** for clustering, using Gaussian Mixture Models (GMM), using both MPI and OpenMP.  

Expectation Maximization is ...


## Prerequisites
- C compiler compatible with C11 or higher (GCC, Clang, or MSVC)
- [CMake](https://cmake.org/download/) (version 3.10 or higher), optional but highly recommended

## Quick Start
To try it immediately you can use the automated demo script:
1. **Clone the repository**
    ```bash
    git clone https://github.com/martinadep/parallel_EM_clustering
    cd parallel_EM_clustering
    ```
2. **Run the script**
    ```bash
    
    ```

## Complete Workflow - example 
1. **Clone the repository**
    ```bash
    git clone https://github.com/martinadep/parallel_EM_clustering
    cd parallel_EM_clustering
    ```

2. **Run the EM algorithm**:

      On Windows (MinGW):
     ```bash
     cmake -B build
     cmake --build build --target em_clustering
     ./build/em_clustering
     ```

      On Linux/macOS:
     ```bash
     cmake -B build
     cmake --build build --target em_clustering
     ./build/em_clustering
     ```
3. (Optional) **Plot the results**

    Use the R script to plot the clustering results. 

## Useful Scripts
Several scripts are provided into `scripts` directory which enable different analysis on EM algorithm, including strong scaling, breakdowns and profiling.


## Performance

Small Dataset: xxx points, xxx dimensions, xxx clusters

| Variant | Sequential Version | Parallel Version | Speedup |
|---------|--------------------|------------------|---------|
| em_omp | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi_omp | 0.00 s       | 0.00 s     | **0.00**   |

---

Medium Dataset: xxx points, xxx dimensions, xxx clusters

| Variant | Sequential Version | Parallel Version | Speedup |
|---------|--------------------|------------------|---------|
| em_omp | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi_omp | 0.00 s       | 0.00 s     | **0.00**   |

---

Big Dataset: xxx points, xxx dimensions, xxx clusters

| Variant | Sequential Version | Parallel Version | Speedup |
|---------|--------------------|------------------|---------|
| em_omp | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi_omp | 0.00 s       | 0.00 s     | **0.00**   |

