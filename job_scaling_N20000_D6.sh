#!/bin/bash
#PBS -N scaling_N20000
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=06:00:00
#PBS -q short_HPC4DS
#PBS -o scaling_N20000_out.txt
#PBS -e scaling_N20000_err.txt

# Load required modules
module load gcc91 openmpi-4.0.4

# Define absolute paths based on the submission directory
PROJECT_ROOT=$PBS_O_WORKDIR
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/N20000_K4_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/scaling_N20000_D6.csv"

# Configuration parameters
K_CLUSTERS=4

# Ensure results directory exists
mkdir -p "$PROJECT_ROOT/results"

echo "=========================================="
echo "STARTING SCALING TEST: N=20000, D=6"
echo "Job Max Time: 6 Hours"
echo "Dataset: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ""
    echo "##########################################"
    echo ">>> Running with $CORES CORES"
    echo "##########################################"
    
    OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null)
    
    echo "$OUTPUT"

    TIME=$(echo "$OUTPUT" | grep "EM_Algorithm execution time" | awk '{print $5}')
    N_PTS=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $5}')
    D_DIM=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $7}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
}

for c in 1 2 4 8 16 32 64; do
    run_test $c
done

echo "=========================================="
echo "Job N20000_D6 Completed."
echo "=========================================="