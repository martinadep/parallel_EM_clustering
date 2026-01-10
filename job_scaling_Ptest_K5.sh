#!/bin/bash
# run_interactive_test.sh
# Script per testare scaling K=5 in sessione interattiva

# 1. Carica i moduli (necessario se è una nuova shell)
module load gcc91 openmpi-4.0.4

# 2. Imposta i percorsi relativi alla directory corrente
PROJECT_ROOT=$(pwd)
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P50000_K5_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/interactive_results_K5.csv"

# Parametri
K_CLUSTERS=5

echo "=========================================="
echo "STARTING INTERACTIVE TEST (K=$K_CLUSTERS)"
echo "Exe: $EXEC_PATH"
echo "Data: $DATASET"
echo "=========================================="

# Verifica esistenza eseguibile
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
    
    # Esegue MPI mostrando l'output a video E salvandolo nel log temporaneo
    # Usa --oversubscribe perché la sessione interattiva potrebbe avere meno core fisici di 64
    mpirun --oversubscribe \
           --mca mca_base_component_show_load_errors 0 \
           --mca btl ^openib \
           --mca btl_base_warn_component_unused 0 \
           -np $CORES "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null 2>&1 | tee "$TMP_LOG"
    
    # Estrae i dati per il CSV
    TIME=$(grep "EM_Algorithm execution time" "$TMP_LOG" | awk '{print $5}')
    N_PTS=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $5}')
    D_DIM=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $7}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

    echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
    
    # Pulizia
    rm "$TMP_LOG"
}

# Esegui il loop
for c in 1 2 4 8 16 32 64; do 
    run_test $c
    # Piccola pausa per leggere l'output se necessario
    sleep 1
done

echo ""
echo "=== TEST COMPLETATO ==="
echo "Risultati salvati in: $RESULT_CSV"