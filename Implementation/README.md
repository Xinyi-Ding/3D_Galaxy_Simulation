# Implementation

This folder contains the implementation of a toy galaxy simulation model, structured into three distinct subfolders, each representing a different stage or approach in the development process. The project is based on the Barnes-Hut N-body simulation algorithm and has been extended for both serial and parallel computation.

## Declaration
The Barnes-Hut implementation for this galaxy simulation is primarily based on the publicly available code by **Justin Rokisky** on [GitLab](https://github.com/Jrokisky/MPI-Barnes-hut/tree/master).

When setting the initialization parameters for galaxy particles, the CUDA version of the code by **James Sandham** on [Bitbucket] (https://bitbucket.org/jsandham/nbodycuda) was referenced to ensure the particles correctly simulate the formation trajectories of galaxies.

## Environment Setup
All implementation and testing were conducted on the `Cirrus` high-performance computing system. 

## How to Use
- Each subfolder contains its own `README` file with specific instructions on how to compile and run the code within that folder.
- Slurm scripts provided in the Parallel Code folder allow for easy submission of jobs on HPC systems with different configurations.