#!/bin/bash
#PBS -N scaling_Ptest_K100
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=06:00:00
#PBS -q short_HPC4DS
#PBS -o scaling_Ptest_K100_out.txt
#PBS -e scaling_Ptest_K100_err.txt

# Load required modules
module load gcc91 openmpi-4.0.4

# Define absolute paths based on the submission directory
PROJECT_ROOT=$PBS_O_WORKDIR
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P50000_K100_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/scaling_Ptest_K100.csv"

# Configuration parameters: K=100
K_CLUSTERS=100

# Ensure results directory exists
mkdir -p "$PROJECT_ROOT/results"

echo "=========================================="
echo "STARTING SCALING TEST: Ptest_K100 (N=50000, K=100, D=6)"
echo "Dataset: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

# Initialize CSV file with header if it does not exist
if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ""
    echo "##########################################"
    echo ">>> Running with $CORES CORES"
    echo "##########################################"
    
    TMP_LOG="run_${CORES}_Ptest_K100.log"
    
    # Run MPI with flags to disable OpenIB (Infiniband) and force TCP/Shared Memory
    mpirun --oversubscribe \
           --mca mca_base_component_show_load_errors 0 \
           --mca btl ^openib \
           --mca btl_base_warn_component_unused 0 \
           -np $CORES "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null 2>&1 | tee "$TMP_LOG"
    
    TIME=$(grep "EM_Algorithm execution time" "$TMP_LOG" | awk '{print $5}')
    N_PTS=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $5}')
    D_DIM=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $7}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
    rm "$TMP_LOG"
}

# Run scaling loop for 1 to 64 cores
for c in 1 2 4 8 16 32 64; do 
    run_test $c
done

echo "=== COMPLETED ==="