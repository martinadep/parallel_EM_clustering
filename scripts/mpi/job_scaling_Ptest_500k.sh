#!/bin/bash
#PBS -N scaling_Ptest_500k
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=06:00:00
#PBS -q short_HPC4DS
#PBS -o scaling_Ptest_500k_out.txt
#PBS -e scaling_Ptest_500k_err.txt

# load required modules
module load gcc91 openmpi-4.0.4

# define absolute paths based on the submission directory
PROJECT_ROOT=$PBS_O_WORKDIR
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P500000_K5_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/scaling_Ptest_500k.csv"

# configuration parameters
K_CLUSTERS=5

# ensure results directory exists
mkdir -p "$PROJECT_ROOT/results"

echo "=========================================="
echo "STARTING SCALING TEST: Ptest (N=500000, D=6)"
echo "Dataset: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

# initialize CSV file with header if it does not exist
if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ""
    echo "##########################################"
    echo ">>> Running with $CORES CORES"
    echo "##########################################"
    
    TMP_LOG="run_${CORES}_Ptest_500k.log"
    
    mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null 2>&1 | tee "$TMP_LOG"
    
    TIME=$(grep "EM_Algorithm execution time" "$TMP_LOG" | awk '{print $5}')
    N_PTS=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $5}')
    D_DIM=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $7}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
    rm "$TMP_LOG"
}

# run scaling loop
for c in 1 2 4 8 16 32 64; do 
    run_test $c
done

echo "=== COMPLETED ==="