#!/bin/bash
# run_interactive_test.sh (OpenMP + Output Visibile)

# 1. Carica i moduli (necessario se è una nuova shell)
module load gcc91 openmpi-4.0.4 2>/dev/null || echo "Module load skipped"

# 2. Imposta i percorsi relativi alla directory corrente
PROJECT_ROOT=$(pwd)
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P50000_K5_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/interactive_results_K5_OMP.csv"

# Parametri
K_CLUSTERS=5

echo "=========================================="
echo "STARTING INTERACTIVE TEST (OpenMP, K=$K_CLUSTERS)"
echo "=========================================="

# Verifica esistenza eseguibile
if [ ! -f "$EXEC_PATH" ]; then
    echo "ERRORE: Eseguibile non trovato in $EXEC_PATH"
    exit 1
fi

mkdir -p "$PROJECT_ROOT/results"
# Inizializza CSV se non esiste
if [ ! -f "$RESULT_CSV" ]; then
    echo "N_Points,K_Clusters,Dimensions,Threads,Time_Sec" > "$RESULT_CSV"
fi

# Variabili per silenziare warning OpenMPI residui (se presenti)
export OMPI_MCA_mca_base_component_show_load_errors=0
export PMIX_MCA_mca_base_component_show_load_errors=0

run_test() {
    THREADS=$1
    echo ""
    echo ">>> Testing with $THREADS THREADS..."
    
    # Imposta numero thread OpenMP
    export OMP_NUM_THREADS=$THREADS
    TMP_LOG="temp_run_${THREADS}.log"

    # USA TEE: Mostra output a video E lo salva nel log
    "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null 2>&1 | tee "$TMP_LOG"
    
    # Prende l'ultima riga (assumendo sia quella CSV prodotta dal main)
    CSV_LINE=$(tail -n 1 "$TMP_LOG")
    
    # Controllo e salvataggio
    if [[ "$CSV_LINE" =~ ^[0-9]+ ]]; then
         echo "$CSV_LINE" >> "$RESULT_CSV"
    else
         echo "WARNING: Formato output non riconosciuto nell'ultima riga."
    fi
    
    rm "$TMP_LOG"
}

# Esegui il loop
# Rileva core logici disponibili per evitare oversubscription inutile (opzionale)
MAX_CORES=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 64)
echo "Detected max cores: $MAX_CORES"

for t in 1 2 4 8 16 32 64; do 
    # (Opzionale) Decommenta per fermarti al numero max di core fisici
    # if [ "$t" -gt "$MAX_CORES" ]; then break; fi
    
    run_test $t
    sleep 0.5
done

echo ""
echo "=== TEST COMPLETATO ==="
echo "Risultati salvati in: $RESULT_CSV"