#!/bin/bash
#PBS -N scaling_3M_2D
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=03:00:00
#PBS -q short_cpuQ
#PBS -o scaling_3M_out.txt
#PBS -e scaling_3M_err.txt

module load gcc91 openmpi-4.0.4

cd $PBS_O_WORKDIR/build

# --- CONFIGURATION ---
DATASET="../datasets/gmm_P3000000_K3_D2.csv"
K_CLUSTERS=3
RESULT_CSV="results_3M_2D_new.csv"
# ---------------------

echo "=========================================="
echo "Starting SCALING TEST: 3M Points"
echo "Dataset: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ""
    echo ">>> Running with $CORES CORES..."
    
    OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES ./em_clustering -d "$DATASET" -k $K_CLUSTERS -o /dev/null)
    
    echo "$OUTPUT"

    # Parsing (estrae Tempo, N e D)
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
echo "Job 3M_2D Completed."
echo "=========================================="