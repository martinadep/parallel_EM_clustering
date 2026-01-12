#!/usr/bin/env bash
set -euo pipefail

# Quick start runner for EM clustering
# 1) configure + build
# 2) run the sequential EM executable
# 3) optionally plot results (if Rscript is available)

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${ROOT_DIR}/build"
RESULTS_DIR="${ROOT_DIR}/results"

echo "== EM Clustering Quick Start =="
echo "Root: ${ROOT_DIR}"
echo "Build dir: ${BUILD_DIR}"
echo

# 1) Configure + build
echo "Configuring and building EM clustering..."
mkdir -p "${BUILD_DIR}"
cmake -S "${ROOT_DIR}" -B "${BUILD_DIR}"
cmake --build "${BUILD_DIR}" --target em_clustering_seq
echo

# 2) Run EM clustering (sequential version)
echo "Running EM clustering (sequential)..."
"${BUILD_DIR}/em_clustering_seq"
echo

# 3) Optional: plot results if Rscript is available
if command -v Rscript >/dev/null 2>&1; then
    echo "Plotting results..."
    Rscript "${ROOT_DIR}/generator_and_analysis/plot_em_results.R"
else
    echo "Rscript not found: skipping plotting."
fi

echo "Done. Check ${RESULTS_DIR} for outputs."
