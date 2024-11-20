#!/bin/sh

#SBATCH --nodes=1
#SBATCH --time=1:30:0
#SBATCH --partition=standard
#SBATCH --qos=standard
#SBATCH --exclusive
#SBATCH --cpus-per-task=1

# Replace [budget code] below with your budget code (e.g. dc116-s1234567)
#SBATCH --account=m23oc-s2507921

module load mpt
echo "mpi loaded"

# Compile the source file with optimization, OpenMP support, and necessary definitions
mpicc mpi_barnes_hut_multi.c particle.c octree.c timer.c -o mpi-barnes-hut-multi -lm
# mpicc mpi_barnes_hut.c particle.c octree.c timer.c -o mpi-barnes-hut -lm


echo "Compilation done"

CSV_FILE="single_node.csv"
if [ ! -f $CSV_FILE ]; then
    echo "Processes,Run1,Run2,Run3,Average" > $CSV_FILE
fi
# Function to run the program and calculate average runtime
run_and_record() {
  local procs=$1
  local ntasks=$((1 * procs))
  echo "Running with $procs processes"
  runtimes=()

  for i in {1..3}; do
    output=$(srun --ntasks=$ntasks --tasks-per-node=$procs ./mpi-barnes-hut-multi 100000 50)
    echo "$output"
    runtime=$(echo "$output" | grep "Run Time" | awk '{print $15}')
    runtimes+=("$runtime")
  done

  sum=0
  for runtime in "${runtimes[@]}"; do
    sum=$(echo "$sum + $runtime" | bc)
  done
  average=$(echo "scale=2; $sum / 3" | bc -l)

  echo "$procs,${runtimes[0]},${runtimes[1]},${runtimes[2]},$average" >> $CSV_FILE
}

# Test different number of processes
for procs in 36 32 24 16 12 8 4 2 1; do
  run_and_record $procs
done
