## README - Parallel Code

This folder contains the parallel implementation of the toy galaxy simulation model.

### Rokisky parallel version:
- **[`mpi_barnes_hut.c`](https://github.com/Jrokisky/MPI-Barnes-hut/blob/master/mpi_barnes_hut.c)**: Rokisky's original MPI implementation designed for running and testing on up to 8 processes.

### Parallel Improvements:
There are two versions of parallel enhancements based on Rokisky's design:
- **`mpi_barnes_hut_multi.c`**: This version is optimized for improving parallel computation performance on distributed memory systems.
- **`mpi_barnes_hut_openmp.c`**: This is an updated version of `mpi_barnes_hut_multi.c` that used hybrid MPI/OpenMP parallelization.

### Slurm Scripts:
Several scripts ending with `.slurm` are provided to test different versions of the programs with various configurations of nodes, MPI processes, and threads. These scripts can also store performance data to CSV files. For detailed performance data, see the [Performance Folder](/Performance/).

Specifically:
- **`mpi.slurm`**: Used for testing the simulation results or running large iterations.
- **`single_node.slurm`**: Tests performance on a single node using different numbers of MPI processes (1, 2, 4, 8, 12, 16, 24, 32, 36).
- **`multi_nodes.slurm`**: Tests performance across multiple nodes (up to 16) with 36 MPI processes. Requires manual modification of the `--nodes` value and corresponding `--ntasks`.
- **`multi_threads.slurm`**: Tests performance on 8 nodes with 1 MPI process and varying numbers of threads (up to 36). Requires manual modification of the `--cpus-per-task` value and the corresponding `OMP_NUM_THREADS`.
- **`strong_scaling.slurm`**: Tests performance across 1, 2, 4, and 8 nodes, using 2 MPI processes per node with 18 threads per process.


### How to Run:
To run the program, you can submit the `mpi.slurm` script using the command:
```bash
sbatch mpi.slurm
```

**Before submitting the job, ensure the following configuration is correct:**
- The value of `--ntasks` should equal the product of `--nodes` and `--tasks-per-node`.
- The product of `--tasks-per-node` and `--cpus-per-task` should not exceed 36.
- The value of `--cpus-per-task` should equal `OMP_NUM_THREADS`.