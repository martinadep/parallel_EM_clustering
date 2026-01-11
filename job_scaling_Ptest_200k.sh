#!/bin/bash

# ==========================================================
# SCRIPT DI SCALING INTERATTIVO (OpenMP)
# ==========================================================
# Uso: ./job_scaling_Ptest_200k.sh
# Questo script deve essere lanciato dalla root del progetto
# o dalla cartella dove risiede lo script.

# 1. Determina la root del progetto
# Se siamo in un job PBS, usa PBS_O_WORKDIR, altrimenti usa la directory dello script
if [ -n "$PBS_O_WORKDIR" ]; then
    PROJECT_ROOT="$PBS_O_WORKDIR"
else
    # Risale alla directory dove si trova questo file
    PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
fi

echo "Project Root: $PROJECT_ROOT"

# Load required modules (silenzioso se fallisce, utile se stai testando in locale)
module load gcc91 openmpi-4.0.4 2>/dev/null || echo "Module load skipped or failed (safe if running locally)"

# 2. Definisci i percorsi
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P200000_K5_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/scaling_Ptest_200k_interactive.csv"

# Configuration parameters
K_CLUSTERS=5

# Ensure results directory exists
mkdir -p "$PROJECT_ROOT/results"

echo "=========================================="
echo "STARTING INTERACTIVE SCALING TEST (OpenMP)"
echo "Dataset: $DATASET"
echo "Saving to: $RESULT_CSV"
echo "=========================================="

# 3. Verifica l'eseguibile
if [ ! -f "$EXEC_PATH" ]; then
    echo "ERRORE: Eseguibile non trovato in $EXEC_PATH"
    echo "Esegui prima 'cmake' e 'make' nella cartella build."
    exit 1
fi

# 4. Inizializza CSV
if [ ! -f "$RESULT_CSV" ]; then
    echo "Threads,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
fi

run_test() {
    THREADS=$1
    echo ""
    echo "##########################################"
    echo ">>> Running with $THREADS THREADS"
    echo "##########################################"
    
    TMP_LOG="run_${THREADS}_Ptest_200k.log"
    
    # Imposta il numero di thread per OpenMP
    export OMP_NUM_THREADS=$THREADS
    
    # Esegue direttamente l'eseguibile (senza mpirun)
    # L'output viene rediretto sul log temporaneo per l'estrazione dei dati
    "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null > "$TMP_LOG" 2>&1
    
    # Parsing dell'output (adattato alle stampe viste nel main.c)
    # Cerca: "*** EM_Algorithm execution time: 1.2345 s ***"
    TIME=$(grep "EM_Algorithm execution time" "$TMP_LOG" | awk '{print $5}')
    
    # Cerca: "[DEBUG] Loaded dataset: 200000 points, 6 dimensions"
    # $4 è 200000, $6 è 6
    N_PTS=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $4}')
    D_DIM=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $6}')
    
    # Controlli di sicurezza
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="N/A"; fi

    echo "Result: $THREADS threads -> $TIME seconds"
    echo "$THREADS,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
    
    rm "$TMP_LOG"
}

# 5. Loop di scaling
# Rileva il numero massimo di core logici disponibili
MAX_CORES=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo 64)
echo "Detected system cores: $MAX_CORES"

# Esegui il test per numero crescente di thread
for t in 1 2 4 8 16 32 64; do
    # Evita di lanciare più thread di quanti core ci sono (opzionale, ma consigliato)
    if [ "$t" -gt "$MAX_CORES" ]; then
        echo "Skipping $t threads (exceeds available cores $MAX_CORES)"
        break
fi
    run_test $t
done
echo "=== COMPLETED ==="