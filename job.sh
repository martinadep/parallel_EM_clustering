#!/bin/bash
#PBS -N em_clustering_job
#PBS -l nodes=1:ppn=4
#PBS -l walltime=00:10:00
#PBS -o output_job.txt
#PBS -e error_job.txt

module load gcc91 openmpi-4.0.4

cd $PBS_O_WORKDIR

cd build

mpirun -np 4 ./em_clustering -d ../datasets/gmm_100000p_3k.csv -k 3 -o ../results/out.csv