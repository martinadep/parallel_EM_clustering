#!/bin/bash
#PBS -N scaling_test_64
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=03:00:00
#PBS -q short_cpuQ
#PBS -o scaling_P50K_K3_D4_out.txt
#PBS -e scaling_64_err.txt

module load gcc91 openmpi-4.0.4

cd $PBS_O_WORKDIR/build

DATASET="../datasets/gmm_P50000_K3_D4.csv" 
K=3
CSV_FILE="../results/scaling_results_P50K_K3_D4.csv"

echo "=========================================="
echo "Starting Scaling Test (1 to 64 cores)"
echo "Dataset: $DATASET"
echo "Results will be saved to: $CSV_FILE"
echo "=========================================="

# Initialize CSV file
echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$CSV_FILE"

run_test() {
    CORES=$1
    echo ""
    echo "##########################################"
    echo "### RUNNING WITH $CORES CORES"
    echo "##########################################"
    
    # Output to log
    OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES ./em_clustering -d $DATASET -k $K -o /dev/null)
    echo "$OUTPUT"
    
    # Parse metrics
    TIME=$(echo "$OUTPUT" | grep "execution time" | awk '{print $(NF-2)}')
    N_POINTS=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $(NF-3)}')
    D_DIM=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $(NF-1)}')
    
    # Append to CSV
    if [ ! -z "$TIME" ]; then
        echo "$CORES,$N_POINTS,$K,$D_DIM,$TIME" >> "$CSV_FILE"
    fi
}

# Run tests sequence
run_test 1
run_test 2
run_test 4
run_test 8
run_test 16
run_test 32
run_test 64

echo "=========================================="
echo "Test Completed. Check $CSV_FILE"
echo "=========================================="
