#!/bin/bash
#PBS -N scaling_test_64
#PBS -l select=4:ncpus=16:mem=4gb
#PBS -l walltime=03:00:00
#PBS -q short_cpuQ
#PBS -o scaling_64_out.txt
#PBS -e scaling_64_err.txt

module load gcc91 openmpi-4.0.4

cd $PBS_O_WORKDIR/build

DATASET="../datasets/gmm_200000p_3k.csv" 

echo "=========================================="
echo "Starting Scaling Test (1 to 64 cores)"
echo "Dataset: $DATASET"
echo "=========================================="

run_test() {
    CORES=$1
    echo ""
    echo "##########################################"
    echo "### RUNNING WITH $CORES CORES"
    echo "##########################################"
    
    # Run standard command showing full output
    mpirun --mca mca_base_component_show_load_errors 0 --oversubscribe -np $CORES ./em_clustering -d $DATASET -k 5 -o /dev/null
}

run_test 1
run_test 2
run_test 4
run_test 8
run_test 16
run_test 32
run_test 64

echo "=========================================="
echo "Test Completed"
echo "=========================================="