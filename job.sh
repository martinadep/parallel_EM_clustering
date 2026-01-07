#!/bin/bash
#PBS -N em_clustering_job
#PBS -l select=1:ncpus=4:mem=2gb
#PBS -l walltime=00:10:00
#PBS -q short_cpuQ
#PBS -o output_job.txt
#PBS -e error_job.txt

module load gcc91 openmpi-4.0.4

cd $PBS_O_WORKDIR
cd build

# Parameters
DATASET="../datasets/gmm_100000p_3k.csv"
K=3
CORES=4
CSV_FILE="../results/single_run_results.csv"

# Initialize CSV header
if [ ! -f "$CSV_FILE" ]; then
    echo "Cores,N_Points,K_Clusters,Dimensions,Time_Sec" > "$CSV_FILE"
fi

# Output to log
OUTPUT=$(mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES ./em_clustering -d $DATASET -k $K -o ../results/out.csv)
echo "$OUTPUT"

# Extract metrics
TIME=$(echo "$OUTPUT" | grep "execution time" | awk '{print $(NF-2)}')
N_POINTS=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $(NF-3)}')
D_DIM=$(echo "$OUTPUT" | grep "Loaded dataset" | awk '{print $(NF-1)}')

# Save to CSV
if [ ! -z "$TIME" ]; then
    echo "$CORES,$N_POINTS,$K,$D_DIM,$TIME" >> "$CSV_FILE"
    echo "Result added to $CSV_FILE"
else
    echo "Error: Could not parse execution time."
fi