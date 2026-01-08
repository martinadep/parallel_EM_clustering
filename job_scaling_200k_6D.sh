#!/bin/bash
#PBS -N scaling_200k_6D
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=06:00:00
#PBS -q short_HPC4DS
#PBS -o scaling_6D_out.txt
#PBS -e scaling_6D_err.txt

module load gcc91 openmpi-4.0.4
cd $PBS_O_WORKDIR/build

# --- CONFIGURATION (UPDATED) ---
DATASET="../datasets/gmm_P200000_K5_D6.csv"
K_CLUSTERS=5
RESULT_CSV="results_200k_6D.csv"

echo "=== STARTING SAFE NIGHT RUN: 6 DIMENSIONS ==="
echo "Dataset: $DATASET"

if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ">>> Running with $CORES CORES..."
    OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES ./em_clustering -d "$DATASET" -k $K_CLUSTERS -o /dev/null)
    
    TIME=$(echo "$OUTPUT" | grep "EM_Algorithm execution time" | awk '{print $5}')
    N_PTS=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $5}')
    D_DIM=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $7}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
}

for c in 1 2 4 8 16 32 64; do run_test $c; done

echo "=== COMPLETED 200k_6D ==="