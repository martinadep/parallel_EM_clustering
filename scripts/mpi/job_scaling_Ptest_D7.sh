#!/bin/bash
# Script to run scaling test for D=7 in an interactive session

# load required modules
module load gcc91 openmpi-4.0.4

# define absolute paths based on the current directory (project root)
PROJECT_ROOT=$(pwd)
EXEC_PATH="$PROJECT_ROOT/build/em_clustering_mpi"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P50000_K5_D7.csv"
RESULT_CSV="$PROJECT_ROOT/results/scaling_Ptest_D7.csv"

K_CLUSTERS=5

echo "=========================================="
echo "STARTING INTERACTIVE TEST (D=7, K=$K_CLUSTERS)"
echo "Exe: $EXEC_PATH"
echo "Data: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

# check if executable exists
if [ ! -f "$EXEC_PATH" ]; then
    echo "ERRORE: Eseguibile non trovato in $EXEC_PATH"
    echo "Assicurati di essere nella root del progetto e di aver compilato."
    exit 1
fi

mkdir -p "$PROJECT_ROOT/results"

# initialize CSV if it does not exist
if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ""
    echo ">>> Testing with $CORES CORES..."
    
    TMP_LOG="interactive_run_${CORES}_D7.log"
    
    # run MPI with safety flags
    mpirun --oversubscribe \
           --mca mca_base_component_show_load_errors 0 \
           --mca btl ^openib \
           --mca btl_base_warn_component_unused 0 \
           -np $CORES "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null 2>&1 | tee "$TMP_LOG"
    
    # extract data for CSV
    TIME=$(grep "EM_Algorithm execution time" "$TMP_LOG" | awk '{print $5}')
    N_PTS=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $5}')
    D_DIM=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $7}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
    
    rm "$TMP_LOG"
}

# scaling loop
for c in 1 2 4 8 16 32 64; do 
    run_test $c
    sleep 1
done

echo ""
echo "=== TEST COMPLETATO ==="
echo "Risultati salvati in: $RESULT_CSV"
