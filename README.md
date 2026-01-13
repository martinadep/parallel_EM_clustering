# Parallel EM Clustering

## Table of Contents

1. [Introduction](#introduction)
2. [Prerequisites](#prerequisites)
3. [Quick Start](#quick-start)
4. [Usage](#usage)
5. [Repository Structure](#repository-structure)
6. [Useful Scripts](#useful-scripts)
7. [Performance](#performance)
8. [Authors](#authors)

## Introduction

This project is for the **High Performance Computing for Data Science** course at the University of Trento (2025).

It involves the **parallelization of the Expectation-Maximization (EM) algorithm** for clustering using Gaussian Mixture Models (GMM). Two parallel implementations are provided:

- **MPI:** Distributed memory parallelization
- **OpenMP:** Shared memory parallelization

The EM algorithm alternates between an **E-step** (computing cluster membership probabilities) and an **M-step** (updating means, covariances, and weights) until convergence.

Tested configurations:
- **N:** 50,000 – 500,000 points
- **D:** 6 – 8 dimensions
- **K:** 5 – 15 clusters

## Prerequisites

- C compiler compatible with C11 or higher (GCC recommended)
- [CMake](https://cmake.org/download/) (version 3.10 or higher)
- MPI library (OpenMPI or MPICH) for the MPI version
- R (optional, for plotting results)

## Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/martinadep/parallel_EM_clustering
   cd parallel_EM_clustering
   ```

2. **Build the project**
   ```bash
   mkdir -p build && cd build
   cmake ..
   make
   ```

3. **Run the algorithm**
   ```bash
   mpirun -np 4 ./build/em_clustering_[version] \
       -d datasets/test/gmm_P50000_K5_D6.csv \
       -k 5 \
       -o results/output.csv
   ```
   Or, using shell scripts in `scripts/` (recommended).

4. **(Optional) Plot the results**
   ```bash
   Rscript generator_and_analysis/plot_em_results.R
   ```

## Usage

### Command Line Flags

| Flag | Description                           |
|------|---------------------------------------|
| `-d` | Input CSV file (required)             |
| `-k` | Number of clusters (required)         |
| `-o` | Output file for results               |
| `-m` | Max iterations (default: 100)         |
| `-t` | Convergence threshold                 |


## Repository Structure

| Folder           | Description                             |
|------------------|-----------------------------------------|
| `src/`           | C source code (EM algorithm, matrix ops)|
| `datasets/test/` | Test CSV datasets                       |
| `results/`       | Benchmark output files                  |
| `scripts/`   | PBS and interactive job scripts         |
| `generator_and_analysis/`     | R scripts for data generation & plotting|

## Useful Scripts

Several scripts are provided in the `scripts/` directory for running scaling tests and profiling, for instance:

| Script                      | Description                      |
|-----------------------------|----------------------------------|
| `job_scaling_Ptest_200k.sh` | Strong scaling test (N=200 000)  |
| `job_scaling_Ptest_K10.sh`  | Scaling test varying K=10        |
| `job_scaling_Ptest_D8.sh`   | Scaling test varying D=8         |

## Authors

- [Martina De Piccoli](https://github.com/martinadep), MSc in Data Science, University of Trento
- [Jago Revrenna](https://github.com/jagorev), MSc in Cloud and Networking Infrastructures and HPC, University of Trento and KTH Royal Institute of Technology