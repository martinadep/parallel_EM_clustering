#!/usr/bin/env bash
set -euo pipefail

# Custom run script for EM clustering - GENERATION ONLY
# Usage: ./custom_run.sh [N] [K] [D]

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DATA_DIR="${ROOT_DIR}/datasets"
LABELS_DIR="${DATA_DIR}/labels"

# Default parameters updated for your request
N=${1:-200000}          # Default: 2000000 data points
K=${2:-7}              # Default: 3 clusters
D=${3:-5}              # Default: 2 dimensions

DATASET_NAME="gmm_P${N}_K${K}_D${D}"
DATASET_PATH="${DATA_DIR}/${DATASET_NAME}.csv"
LABELS_PATH="${LABELS_DIR}/${DATASET_NAME}_labels.csv"

echo "=========================================================="
echo "              Dataset Generation Only"
echo "=========================================================="
echo "Parameters:"
echo "  N (points):    ${N}"
echo "  K (clusters):  ${K}"
echo "  D (dimensions): ${D}"
echo "=========================================================="
echo

# Ensure output directories exist
mkdir -p "${DATA_DIR}" "${LABELS_DIR}"

# Generate dataset if necessary
if [ ! -f "${DATASET_PATH}" ] || [ ! -f "${LABELS_PATH}" ]; then
    echo "Generating dataset..."
    if command -v Rscript >/dev/null 2>&1; then
        Rscript "${ROOT_DIR}/generator_and_analysis/data_generator.R" "${N}" "${K}" "${D}"
        echo ">>> Dataset generated at: ${DATASET_PATH}"
    else
        echo "ERROR: Rscript not found. Cannot generate dataset."
        exit 1
    fi
else
    echo "Dataset already exists at: ${DATASET_PATH}"
fi

echo "Done."
exit 0