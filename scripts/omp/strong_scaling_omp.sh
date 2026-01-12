#!/usr/bin/env bash
set -euo pipefail

# Strong scaling script for OpenMP EM clustering
# Usage: ./strong_scaling_omp.sh [N] [K] [D]

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
DATA_DIR="${ROOT_DIR}/datasets/test"
LABELS_DIR="${DATA_DIR}/labels"
RESULTS_DIR="${ROOT_DIR}/results/omp"
PLOTS_DIR="${RESULTS_DIR}/plots"

# Default parameters
N=${1:-10000}
K=${2:-3}
D=${3:-2}
EXECUTABLE="em_clustering_omp"

DATASET_NAME="gmm_P${N}_K${K}_D${D}"
DATASET_PATH="${DATA_DIR}/${DATASET_NAME}.csv"
EXECUTABLE_PATH="${BUILD_DIR}/${EXECUTABLE}"

echo "=========================================================="
echo "              EM Clustering - OpenMP Strong Scaling"
echo "=========================================================="
echo "Parameters:"
echo "  N (points):    ${N}"
echo "  K (clusters):  ${K}"
echo "  D (dimensions): ${D}"
echo "  Executable:    ${EXECUTABLE}"
echo "=========================================================="
echo

# Ensure output directories exist
mkdir -p "${RESULTS_DIR}"

# 1) Build if necessary
if [ ! -f "${EXECUTABLE_PATH}" ]; then
    echo "[1/2] Building project (executable not found)..."
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
    cmake --build "${BUILD_DIR}" --target "${EXECUTABLE}"
    echo ">>> Build completed"
else
    echo "[1/2] Executable found, skipping build"
fi
echo

# 2) Run strong scaling loop
echo "[2/2] Running strong scaling tests..."
for THREADS in 1 2 4 8 16 32 64
do
    echo "Running EM clustering with ${THREADS} threads..."
    OMP_NUM_THREADS=${THREADS} "${EXECUTABLE_PATH}" \
        -d "${DATASET_PATH}" \
        -k "${K}" \
        -o "${RESULTS_DIR}/em_P${N}_K${K}_D${D}_threads${THREADS}.csv" \
        >> "${RESULTS_DIR}/strong_scaling_omp_results_P${N}_K${K}_D${D}.txt"
done

echo "=========================================================="
echo "       *** All steps completed successfully! ***" 
echo "=========================================================="
