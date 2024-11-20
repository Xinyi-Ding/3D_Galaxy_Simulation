#!/bin/sh

#SBATCH --nodes=1
#SBATCH --time=0:20:0
#SBATCH --partition=standard
#SBATCH --qos=short
#SBATCH --exclusive
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=1

# Replace [budget code] below with your budget code (e.g. dc116-s1234567)
#SBATCH --account=m23oc-s2507921

module load gcc
echo "GCC loaded"
# Compile the source file with optimization, OpenMP support, and necessary definitions
gcc -g serial_barnes_hut.c particle.c octree.c timer.c -o serial-barnes-hut -lm
echo "Compilation done"
# Initialize the number of bodies (K) and number of iterations (N)
K=100000
N=50
echo "Number of bodies: $K, Number of iterations: $N"
# Run the parallel program
./serial-barnes-hut $K $N