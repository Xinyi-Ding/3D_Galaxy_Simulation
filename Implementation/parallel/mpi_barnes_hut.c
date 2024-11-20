/******************************************************************************
 * Implementation of the Barnes Hut Algorithm using MPI.
 *
 * Author: Justin Rokisky
 * Modified by: Xinyi Ding 10/08/2024
 * 
 * To compile: mpicc mpi_barnes_hut.c particle.c octree.c timer.c -o mpi-barnes-hut -lm
 * To run: srun ./mpi-barnes-hut NUM_PARTICLES NUM_TIMESTEPS
 *
 * Design decisions:
 *  -> Currently only runs with 8 processes
 *  -> Each process builds a copy of the tree on its machine
 *  -> Each process independently takes one of the 8 subtrees and updates the
 *     position and velocity of the leafs in that tree.
 *  -> The tree is constructed from the bottom up. At each level, particles are
 *     split in the octants they will end up in, and then recursively passed
 *     into the build_octree function.My original plan was to parallelize the
 *     tree construction but was unable to build a data structure that could
 *     accomplish passing the tree pieces.
 *
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
    // Process 0 generates and distributes random particles.
    if (rank == 0)
    {
        srand(time(NULL));
        // Generate random particles.
        double galaxy_thickness = 5.0;
        generate_random_particles(particle_array, space, npart, galaxy_thickness);

        // Broadcast particle data.
        MPI_Bcast(particle_array, npart, mpi_particle_type, 0, MPI_COMM_WORLD);
    }
    else
    {
        // Get random particles from root node.
        MPI_Bcast(particle_array, npart, mpi_particle_type, 0, MPI_COMM_WORLD);
    }

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
        int endpos = rank == (size - 1) ? npart : rank * chunk + chunk;

        // compute forces
        for (int idx = startpos; idx < endpos; idx++)
        {
            Particle *tmp_p = &(particle_array[idx]);
            tmp_p->force_x = 0.0;
            tmp_p->force_y = 0.0;
            tmp_p->force_z = 0.0;
            if (in_space(space, particle_array[idx]))
            {
                compute_force(tmp_p, octree);
            }
            update_particle_position_and_velocity(&particle_array[idx]);
        }

        // No longer need the octree.
        free_octree(octree);
        // Share this processes updated particle data with everyone else.
        for (int q = 0; q < size; q++)
        {
            // last chunk.
            int tmp_chunk = (q == (size - 1)) ? last_chunk : chunk;

            Particle *updated_particles = (Particle *)malloc(tmp_chunk * sizeof(Particle));

            if (q == rank)
            {
                for (int f = 0; f < tmp_chunk; f++)
                {
                    int idx = rank * chunk + f;
                    updated_particles[f].id = particle_array[idx].id;
                    updated_particles[f].x = particle_array[idx].x;
                    updated_particles[f].y = particle_array[idx].y;
                    updated_particles[f].z = particle_array[idx].z;
                    updated_particles[f].vel_x = particle_array[idx].vel_x;
                    updated_particles[f].vel_y = particle_array[idx].vel_y;
                    updated_particles[f].vel_z = particle_array[idx].vel_z;
                }
            }

            // Perform the broadcast.
            MPI_Bcast(updated_particles, tmp_chunk, mpi_particle_type, q, MPI_COMM_WORLD);

            if (q != rank)
            {
                // Update particles.
                for (int k = 0; k < tmp_chunk; k++)
                {
                    // Get the index of the particle that needs updating.
                    int idx = updated_particles[k].id;
                    if (particle_array[idx].id == idx)
                    {
                        particle_array[idx].x = updated_particles[k].x;
                        particle_array[idx].y = updated_particles[k].y;
                        particle_array[idx].z = updated_particles[k].z;
                        particle_array[idx].vel_x = updated_particles[k].vel_x;
                        particle_array[idx].vel_y = updated_particles[k].vel_y;
                        particle_array[idx].vel_z = updated_particles[k].vel_z;
                    }
                    else
                    {
                        printf("WOT\n");
                    }
                }
            }
            free(updated_particles);
        }
        if (rank == 0)
        {
            char filename[256];
            snprintf(filename, sizeof(filename), "pos/positions_%d.txt", i);
            writePositions(filename, particle_array, npart);
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Free Particles.
    free(particle_array);
    endtime = MPI_Wtime() - starttime;
    if (rank == 0)
        printf("Processor Count: %d | Num Particles: %d | Num Steps: %d | Run Time: %f |\n", size, npart, t_step, endtime);
    MPI_Finalize();
    return 0;
}