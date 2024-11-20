#!/bin/sh

#SBATCH --nodes=1
#SBATCH --time=0:10:0
#SBATCH --partition=standard
#SBATCH --qos=short
#SBATCH --exclusive
#SBATCH --tasks-per-node=1
#SBATCH --cpus-per-task=1

# Replace [budget code] below with your budget code (e.g. dc116-s1234567)
#SBATCH --account=m23oc-s2507921

module load oneapi
module load intel-20.4/compilers

# Set the source file and executable name
SRC=nbody.c
EXE=nbody

# Compile the source file with optimization, OpenMP support, and necessary definitions
gcc -std=c99 -O3 -fopenmp -o $EXE $SRC -lm

# Print the name of the executable
echo $EXE

# Initialize the number of bodies (K)
K=1000

# Run the parallel program
./$EXE $K