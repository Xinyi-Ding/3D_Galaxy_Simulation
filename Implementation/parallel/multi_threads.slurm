#!/bin/sh

#SBATCH --nodes=8
#SBATCH --time=0:20:0
#SBATCH --partition=standard
#SBATCH --qos=standard
#SBATCH --exclusive
#SBATCH --ntasks=16
#SBATCH --tasks-per-node=2
#SBATCH --cpus-per-task=18

# Replace [budget code] below with your budget code (e.g. dc116-s1234567)
#SBATCH --account=m23oc-s2507921

module load mpt
echo "mpi loaded"

# Compile the source file with optimization, OpenMP support, and necessary definitions
mpicc -fopenmp mpi_barnes_hut_openmp.c particle.c octree.c timer.c -o mpi-barnes-hut-openmp -lm

echo "Compilation done"

export OMP_NUM_THREADS=18

CSV_FILE="multi_threads_8nodes.csv"
if [ ! -f $CSV_FILE ]; then
    echo "Nodes,Processes,Threads,Run1,Run2,Run3,Average" > $CSV_FILE
fi
# Initialize an empty array to store runtimes
runtimes=()

# Loop to run the program three times
for i in {1..3}; do
  output=$(srun ./mpi-barnes-hut-openmp 100000 50)
  echo "$output"
  runtime=$(echo "$output" | grep "Run Time" | awk '{print $15}')
  runtimes+=("$runtime")
done

# Calculate the average runtime
sum=0
for runtime in "${runtimes[@]}"; do
  sum=$(echo "$sum + $runtime" | bc)
done
average=$(echo "scale=2; $sum / 3" | bc -l)

# Append the results to the CSV file
echo "8,2,18,${runtimes[0]},${runtimes[1]},${runtimes[2]},$average" >> $CSV_FILE
