#!/bin/bash
#PBS -N scaling_2M_2D
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=06:00:00
#PBS -q short_HPC4DS
#PBS -o scaling_2M_out.txt
#PBS -e scaling_2M_err.txt

# Load required modules
module load gcc91 openmpi-4.0.4

# Define absolute paths based on the submission directory
PROJECT_ROOT=$PBS_O_WORKDIR
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/gmm_P2000000_K3_D2.csv"
RESULT_CSV="$PROJECT_ROOT/results/results_2M_2D.csv"

# Configuration parameters
K_CLUSTERS=3

# Ensure results directory exists
mkdir -p "$PROJECT_ROOT/results"

echo "=========================================="
echo "STARTING SCALING TEST: 2M POINTS, 2D"
echo "Job Max Time: 6 Hours"
echo "Executable: $EXEC_PATH"
echo "Dataset: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

# Initialize CSV file with header if it does not exist
if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

# Function to execute test for a specific number of cores
run_test() {
    CORES=$1
    echo ""
    echo "##########################################"
    echo ">>> Running with $CORES CORES"
    echo "##########################################"
    
    # Run MPI command and capture output
    OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null)
    
    # Print the full output to the .txt log file
    echo "$OUTPUT"

    # Parse output values for CSV
    TIME=$(echo "$OUTPUT" | grep "EM_Algorithm execution time" | awk '{print $5}')
    N_PTS=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $5}')
    D_DIM=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $7}')
    
    # Handle missing values or errors
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    # Append results to CSV
    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
}

# Execute scaling loop
for c in 1 2 4 8 16 32 64; do
    run_test $c
done

echo "=========================================="
echo "Job 2M_2D Completed."
echo "=========================================="