#!/usr/bin/env bash
#!/usr/bin/env bash
set -euo pipefail

# Custom run script for EM clustering
# Usage: ./custom_run.sh [N] [K] [D] [EXECUTABLE_NAME]
# Example: ./custom_run.sh 100000 3 2

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
DATA_DIR="${ROOT_DIR}/datasets"
LABELS_DIR="${DATA_DIR}/labels"
RESULTS_DIR="${ROOT_DIR}/results"
PLOTS_DIR="${RESULTS_DIR}/plots"

# Default parameters
N=${1:-10000}          # Number of data points
K=${2:-3}              # Number of clusters
D=${3:-2}              # Dimensions
EXECUTABLE=${4:-"em_clustering_omp"}

DATASET_NAME="gmm_P${N}_K${K}_D${D}"
DATASET_PATH="${DATA_DIR}/${DATASET_NAME}.csv"
LABELS_PATH="${LABELS_DIR}/${DATASET_NAME}_labels.csv"
EXECUTABLE_PATH="${BUILD_DIR}/${EXECUTABLE}"

echo "=========================================================="
echo "              EM Clustering - Custom Run"
echo "=========================================================="
echo "Parameters:"
echo "  N (points):    ${N}"
echo "  K (clusters):  ${K}"
echo "  D (dimensions): ${D}"
echo "  Executable:    ${EXECUTABLE}"
echo "=========================================================="
echo

# Ensure output directories exist
mkdir -p "${DATA_DIR}" "${LABELS_DIR}" "${RESULTS_DIR}" "${PLOTS_DIR}"

# 1) Build if necessary
if [ ! -f "${EXECUTABLE_PATH}" ]; then
    echo "[1/4] Building project (executable not found)..."
    cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
    cmake --build "${BUILD_DIR}" --target "${EXECUTABLE}"
    echo ">>> Build completed"
else
    echo "[1/4] Executable found, skipping build"
fi
echo

# 2) Generate dataset if necessary
if [ ! -f "${DATASET_PATH}" ] || [ ! -f "${LABELS_PATH}" ]; then
    echo "[2/4] Generating dataset..."
    if command -v Rscript >/dev/null 2>&1; then
        Rscript "${ROOT_DIR}/generator_and_analysis/data_generator.R" "${N}" "${K}" "${D}"
        echo ">>> Dataset generated"
    else
        echo "ERROR: Rscript not found. Cannot generate dataset."
        echo "Please install R or generate the dataset manually."
        exit 1
    fi
else
    echo "[2/4] Dataset already exists, skipping generation"
fi
echo

for THREADS in 1 2 4 8 16 32 64 96
do
    echo "[3/4] Running EM clustering with ${THREADS} threads..."
    echo "Command: OMP_NUM_THREADS=${THREADS} ${EXECUTABLE_PATH}"
    OMP_NUM_THREADS=${THREADS} "${EXECUTABLE_PATH}" -d "${DATASET_PATH}" -k "${K}" -o "${RESULTS_DIR}/em_P${N}_K${K}_D${D}_threads${THREADS}.csv" >> "${RESULTS_DIR}/strong_scaling_omp_results.csv"
    echo
done

echo "=========================================================="
echo "       *** All steps completed successfully! ***" 
echo "=========================================================="
