/******************************************************************************
* Updated version of mpi_barnes_hut.c for improve communication between nodes.
* 
* To compile: mpicc mpi_barnes_hut_multi.c particle.c octree.c timer.c -o mpi-barnes-hut-multi -lm
* To run: srun ./mpi-barnes-hut-multi NUM_PARTICLES NUM_TIMESTEPS
*
* Main differences:
*  -> After updating particles, gather all updated particles at root process.
 *****************************************************************************/

#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <stdbool.h>
#include <stddef.h>

#include "particle.h"
#include "octree.h"

#define WIDTH 99999.0
#define LENGTH 99999.0
#define HEIGHT 99999.0

#define ORIGIN_X -99999.0
#define ORIGIN_Y -99999.0
#define ORIGIN_Z -99999.0

void writePositions(const char *filename, Particle *p, int nBodies)
{
    FILE *file = fopen(filename, "w");
    for (int i = 0; i < nBodies; i++)
    {
        fprintf(file, "%f %f %f\n", p[i].x, p[i].y, p[i].z);
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    int npart, t_step, rank, size;
    double starttime, endtime;

    // Init MPI.
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype mpi_particle_type;

    // Create MPI_Datatype for sending particles.
    MPI_Aint displacements_c[11] = {
        offsetof(Particle, id),
        offsetof(Particle, x),
        offsetof(Particle, y),
        offsetof(Particle, z),
        offsetof(Particle, vel_x),
        offsetof(Particle, vel_y),
        offsetof(Particle, vel_z),
        offsetof(Particle, force_x),
        offsetof(Particle, force_y),
        offsetof(Particle, force_z),
        offsetof(Particle, mass)};

    int block_lengths_c[11] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype dtypes_c[11] = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Type_create_struct(11, block_lengths_c, displacements_c, dtypes_c, &mpi_particle_type);
    MPI_Type_commit(&mpi_particle_type);

    // Default space.
    Space space = {WIDTH, LENGTH, HEIGHT, ORIGIN_X, ORIGIN_Y, ORIGIN_Z};

    starttime = MPI_Wtime();
    // Process user input
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s n timesteps\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    npart = atoi(argv[1]);
    t_step = atoi(argv[2]);

    Particle *particle_array = (Particle *)malloc(npart * sizeof(Particle));
    for (int i = 0; i < npart; i++)
    {
        // init empty.
        particle_array[i].id = 0;
        particle_array[i].x = 0.0;
        particle_array[i].y = 0.0;
        particle_array[i].z = 0.0;
        particle_array[i].vel_x = 0.0;
        particle_array[i].vel_y = 0.0;
        particle_array[i].vel_z = 0.0;
        particle_array[i].force_x = 0.0;
        particle_array[i].force_y = 0.0;
        particle_array[i].force_z = 0.0;
        particle_array[i].mass = 0.0;
    }
    if (rank == 0)
    {
        srand(time(NULL));
        // Generate random particles.
        double galaxy_thickness = 5.0;
        generate_random_particles(particle_array, space, npart, galaxy_thickness);
    }

    // Broadcast particle data to all processes.
    MPI_Bcast(particle_array, npart, mpi_particle_type, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < t_step; i++)
    {
        // Each process builds the whole tree and computes centers of mass.
        Octree *octree = create_empty_octree(space);
        for (int j = 0; j < npart; j++)
        {
            if (in_space(space, particle_array[j]))
            {
                octree_insert(octree, space, &(particle_array[j]));
            }
        }

        int chunk = 0;
        int last_chunk = 0;
        // Split particles into chunks.
        if ((npart / size) == 0)
        {
            chunk = npart / size;
            last_chunk = chunk;
        }
        else
        {
            chunk = npart / size;
            last_chunk = npart - ((size - 1) * chunk);
        }

        // Each process processes its chunk.
        int startpos = rank * chunk;
        int endpos = (rank == size - 1) ? npart : startpos + chunk;

        // Compute forces and update particle positions.
        for (int idx = startpos; idx < endpos; idx++)
        {
            Particle *tmp_p = &(particle_array[idx]);
            tmp_p->force_x = 0.0;
            tmp_p->force_y = 0.0;
            tmp_p->force_z = 0.0;
            if (in_space(space, *tmp_p))
            {
                compute_force(tmp_p, octree);
            }
            update_particle_position_and_velocity(tmp_p);
        }

        // No longer need the octree.
        free_octree(octree);

        // Gather all updated particles at root process.
        MPI_Gather(&particle_array[startpos], chunk, mpi_particle_type, particle_array, chunk, mpi_particle_type, 0, MPI_COMM_WORLD);

        // Broadcast the updated particles to all processes.
        MPI_Bcast(particle_array, npart, mpi_particle_type, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            char filename[256];
            snprintf(filename, sizeof(filename), "pos/positions_%d.txt", i);
            writePositions("particle_positions.txt", particle_array, npart);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    // Free Particles.
    free(particle_array);

    endtime = MPI_Wtime() - starttime;
    if (rank == 0)
    {
        printf("Processor Count: %d | Num Particles: %d | Num Steps: %d | Run Time: %f |\n", size, npart, t_step, endtime);
    }
    MPI_Finalize();
    return 0;
}