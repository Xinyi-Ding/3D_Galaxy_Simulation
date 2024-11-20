#!/bin/sh

#SBATCH --nodes=4
#SBATCH --time=1:00:0
#SBATCH --partition=standard
#SBATCH --qos=standard
#SBATCH --exclusive
#SBATCH --ntasks=8
#SBATCH --tasks-per-node=2
#SBATCH --cpus-per-task=18

# Replace [budget code] below with your budget code (e.g. dc116-s1234567)
#SBATCH --account=m23oc-s2507921

module load mpt
echo "mpi loaded"

# Compile different versions of the program: Rokisky MPI ver, updated MPI ver, MPI with OpenMP
# mpicc mpi_barnes_hut.c particle.c octree.c timer.c -o mpi-barnes-hut -lm
# mpicc mpi_barnes_hut_multi.c particle.c octree.c timer.c -o mpi-barnes-hut-multi -lm
mpicc -fopenmp mpi_barnes_hut_openmp.c particle.c octree.c timer.c -o mpi-barnes-hut-openmp -lm

# Make sure to set the number of threads to the number of cores per task
export OMP_NUM_THREADS=18

# Run the program
srun ./mpi-barnes-hut-openmp 10000 1000