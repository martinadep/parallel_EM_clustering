#!/bin/bash

# ==========================================================
# SCRIPT DI SCALING INTERATTIVO (OpenMP)
# ==========================================================
# Uso: ./job_scaling_Ptest_200k.sh

# 1. Determina la root del progetto
if [ -n "$PBS_O_WORKDIR" ]; then
    PROJECT_ROOT="$PBS_O_WORKDIR"
else
    PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
fi

echo "Project Root: $PROJECT_ROOT"

# Load required modules
module load gcc91 openmpi-4.0.4 2>/dev/null || echo "Module load skipped"

# 2. Definisci i percorsi
EXEC_PATH="$PROJECT_ROOT/build/em_clustering"
DATASET="$PROJECT_ROOT/datasets/test/gmm_P200000_K5_D6.csv"
RESULT_CSV="$PROJECT_ROOT/results/scaling_Ptest_200k_interactive.csv"
K_CLUSTERS=5

mkdir -p "$PROJECT_ROOT/results"

echo "=========================================="
echo "STARTING INTERACTIVE SCALING TEST (OpenMP)"
echo "Dataset: $DATASET"
echo "Saving summary to: $RESULT_CSV"
echo "=========================================="

# 3. Verifica l'eseguibile
if [ ! -f "$EXEC_PATH" ]; then
    echo "ERRORE: Eseguibile non trovato in $EXEC_PATH"
    echo "Esegui 'make' nella cartella build."
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
    
    # Esegue l'algoritmo.
    # 'tee' mostra l'output a schermo E lo salva nel log temporaneo per il parsing
    # -o /dev/null perché non vogliamo salvare un file CSV di output, ma vogliamo vedere i print su schermo
    "$EXEC_PATH" -d "$DATASET" -k $K_CLUSTERS -o /dev/null 2>&1 | tee "$TMP_LOG"
    
    # Parsing per il file CSV riassuntivo
    TIME=$(grep "EM_Algorithm execution time" "$TMP_LOG" | awk '{print $5}')
    N_PTS=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $4}')
    D_DIM=$(grep "Loaded dataset" "$TMP_LOG" | awk '{print $6}')
    
    if [ -z "$TIME" ]; then TIME="ERR"; fi
    if [ -z "$N_PTS" ]; then N_PTS="N/A"; fi

    # Salva solo i tempi nel CSV riassuntivo
    echo "$THREADS,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
    
    rm "$TMP_LOG"
}

# Rilevamento core (ignora limite per test forzato)
MAX_CORES=$(nproc 2>/dev/null || sysctl -n hw.logicalcpu 2>/dev/null || echo "Unknown")
echo "Detected system cores: $MAX_CORES (ignoring limit)"

# 5. Loop di scaling
# Rimosso il controllo che bloccava il loop
for t in 1 2 4 8 16 32 64; do
    run_test $t
done

echo "=== COMPLETED ==="