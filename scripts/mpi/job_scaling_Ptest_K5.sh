#!/bin/bash
# Script to test scaling K=5 in an interactive session

# load modules (needed for a fresh shell)
module load gcc91 openmpi-4.0.4

# set paths relative to the current directory
PROJECT_ROOT=$(pwd)
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P50000_K5_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/interactive_results_K5.csv"

K_CLUSTERS=5

echo "=========================================="
echo "STARTING INTERACTIVE TEST (K=$K_CLUSTERS)"
echo "Exe: $EXEC_PATH"
echo "Data: $DATASET"
echo "=========================================="

# check if executable exists
if [ ! -f "$EXEC_PATH" ]; then
    echo "ERRORE: Eseguibile non trovato in $EXEC_PATH"
    echo "Prova a ricompilare o spostarti nella root del progetto."
    exit 1
fi

mkdir -p "$PROJECT_ROOT/results"
if [ ! -f "$RESULT_CSV" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    CORES=$1
    echo ""
    echo ">>> Testing with $CORES CORES..."
    
    TMP_LOG="interactive_run_${CORES}.log"
    
    # run MPI and show output while saving it to a temporary log
    # use --oversubscribe because the interactive session may have fewer than 64 physical cores
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
    
    # cleanup
    rm "$TMP_LOG"
}

# run the loop
for c in 1 2 4 8 16 32 64; do 
    run_test $c
    sleep 1
done

echo ""
echo "=== TEST COMPLETATO ==="
echo "Risultati salvati in: $RESULT_CSV"
