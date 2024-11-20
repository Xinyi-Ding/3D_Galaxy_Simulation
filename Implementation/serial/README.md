## README - Serial Code

This folder contains a serial version of a galaxy simulation toy model, developed based on the Barnes-Hut N-body simulation structure by Rokisky ([Rokisky's MPI-Barnes-Hut repository](https://github.com/Jrokisky/MPI-Barnes-hut/tree/master)). The code has been modified to simulate the motion of a disk-like galaxy rather than particles in random squre space.

### The `pos` Folder
This folder is pre-created to store the positions of particles for each iteration of the simulation.

### How to Run the Code

1. **Using SLURM:**
   - You can submit the provided SLURM script. Make sure to replace the account with a valid budget code.

2. **Compiling and Running Manually:**
   - Ensure that `gcc` is installed and loaded on your device.
   - Compile the program using the following command:
     ```
     gcc -g serial_barnes_hut.c particle.c octree.c timer.c -o serial-barnes-hut -lm
     ```
   - Run the program with the following command:
     ```
     ./serial-barnes-hut 1000 100
     ```
     - The first parameter (`1000` in the example) specifies the size of the galaxy, determining how many particles are included in the simulation.
     - The second parameter (`100` in the example) specifies the number of iterations the simulation will run.