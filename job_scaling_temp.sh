#!/bin/bash
#PBS -N scaling_night_run
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=03:00:00
#PBS -q short_cpuQ
#PBS -o scaling_night_out.txt
#PBS -e scaling_night_err.txt

# Carica i moduli necessari
module load gcc91 openmpi-4.0.4

# Spostati nella cartella build
cd $PBS_O_WORKDIR/build

# Funzione per eseguire la suite di scaling su un dataset specifico
# Argomenti: 1=Dataset Path, 2=K clusters, 3=Output CSV Name
run_scaling_suite() {
    DATASET_PATH=$1
    K_CLUSTERS=$2
    RESULT_CSV=$3

    echo "=================================================================="
    echo "STARTING SUITE FOR: $DATASET_PATH (K=$K_CLUSTERS)"
    echo "SAVING TO: $RESULT_CSV"
    echo "=================================================================="

    # Inizializza il file CSV con l'header se non esiste
    if [ ! -f "$RESULT_CSV" ]; then
        echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$RESULT_CSV"
    fi

    # Loop da 1 a 64 cores
    for CORES in 1 2 4 8 16 32 64; do
        echo ""
        echo ">>> Running with $CORES cores..."
        
        # Esegui mpirun catturando l'output in una variabile
        OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES ./em_clustering -d "$DATASET_PATH" -k $K_CLUSTERS -o /dev/null)
        
        # Stampa l'output nel log PBS per debug
        echo "$OUTPUT"

        # --- PARSING OUTPUT ---
        # Estrae il tempo (cerca la stringa "*** EM_Algorithm execution time: X s ***")
        # Token $5 è il tempo numerico
        TIME=$(echo "$OUTPUT" | grep "EM_Algorithm execution time" | awk '{print $5}')
        
        # Estrae N e D (cerca "[MPI Master] Loaded dataset: N points, D coordinates")
        # Token $5 è N, Token $7 è D
        N_PTS=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $5}')
        D_DIM=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $7}')
        
        # Se il parsing fallisce (es. errore MPI), metti "ERR"
        if [ -z "$TIME" ]; then TIME="ERR"; fi
        if [ -z "$N_PTS" ]; then N_PTS="Missing"; fi

        # Appende la riga al CSV
        echo "$CORES,$N_PTS,$K_CLUSTERS,$D_DIM,$TIME" >> "$RESULT_CSV"
        echo ">>> Saved result: $TIME sec"
    done
}

# ==========================================
# ESECUZIONE TEST NOTTURNI
# ==========================================

# 1. Dataset BIG DATA (3 Milioni di punti, 3 cluster, 2D)
# NB: Assicurati che il file esista a questo percorso
run_scaling_suite "../datasets/gmm_P3000000_K3_D2.csv" 3 "results_3M_2D.csv"

# 2. Dataset HIGH DIMENSIONS (500k punti, 5 cluster, 8D)
# NB: Assicurati che il file esista a questo percorso
run_scaling_suite "../datasets/gmm_P500000_K5_D8.csv" 5 "results_500k_8D.csv"

echo "=========================================="
echo "Nightly Tests Completed."
echo "=========================================="