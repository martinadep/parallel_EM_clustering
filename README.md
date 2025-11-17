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
To try it immediately you can run the automated demo script:

```bash
# Clone the repository
git clone https://github.com/martinadep/parallel_EM_clustering
cd parallel_EM_clustering
```
 
## Useful Scripts
Several scripts are provided into `scripts` directory which enable different analysis on EM algorithm, including strong scaling, breakdowns and profiling.


## Performance

Considering ...:

| Variant | Sequential Version | Parallel Version | Speedup |
|---------|--------------------|------------------|---------|
| em_omp | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi | 0.00 s       | 0.00 s     | **0.00**    |
| em_mpi_omp | 0.00 s       | 0.00 s     | **0.00**   |

