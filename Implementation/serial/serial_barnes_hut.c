#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <float.h>
#include <math.h> // Added for sqrtf and M_PI

#include "particle.h"
#include "octree.h"
#include "timer.h"

#define WIDTH 99999.0
#define LENGTH 99999.0
#define HEIGHT 99999.0

#define ORIGIN_X -99999.0
#define ORIGIN_Y -99999.0
#define ORIGIN_Z -99999.0

void writePositions(const char* filename, Particle* p, int nBodies) {
    FILE* file = fopen(filename, "w");
    for (int i = 0; i < nBodies; i++) {
        fprintf(file, "%f %f %f\n", p[i].x, p[i].y, p[i].z);
    }
    fclose(file);
}

int main(int argc, char *argv[]) {
    int npart, t_step;
    float dt = 0.01f; // Time step for integration
    float smoothing_length = 1.0f;

    // Default space
    Space space = {WIDTH, LENGTH, HEIGHT, ORIGIN_X, ORIGIN_Y, ORIGIN_Z}; 

    // Process user input
    if (argc < 3) {  
        fprintf(stderr, "Usage: %s n timesteps\n", argv[0]);
        return 1;
    } 
    npart = atoi(argv[1]); 
    t_step = atoi(argv[2]);

    Particle *particle_array = (Particle *) malloc(npart * sizeof(Particle));
    for (int i = 0; i < npart; i++) {
        // Initialize empty particles
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
        particle_array[i].mass = 1.0; // Assuming mass of 1.0 for simplicity
    }

    srand(time(NULL));
    // Generate random particles
    float galaxy_diameter = 100.0f;
    float galaxy_thickness = 5.0f;
    generate_random_particles(particle_array, space, npart, galaxy_diameter, galaxy_thickness);
    for (int i = 0; i < t_step; i++) {
        StartTimer(); // Start the timer

        // Each process builds the whole tree and computes centers of mass
        Octree *octree = create_empty_octree(space);
        for (int j = 0; j < npart; j++) {
            if (in_space(space, particle_array[j])) {
                octree_insert(octree, space, &(particle_array[j]));
            }
        }
        
        // Compute forces
        for (int idx = 0; idx < npart; idx++) {
            Particle *tmp_p = &(particle_array[idx]);
            tmp_p->force_x = 0.0;
            tmp_p->force_y = 0.0;
            tmp_p->force_z = 0.0;
            if (in_space(space, particle_array[idx])) {
                compute_force(tmp_p, octree);
            }
        }

        // Update particle positions
        for (int i = 0; i < npart; i++) {
            update_particle_position_and_velocity(&particle_array[i]);
        }

        // No longer need the octree
        free_octree(octree);

        double elapsed = GetTimer(); // Get the elapsed time
        printf("STEP: %d completed in %.3f seconds\n", i, elapsed);

        char filename[256];
        snprintf(filename, sizeof(filename), "pos/positions_%d.txt", i);
        writePositions(filename, particle_array, npart);
    }

    // Free particles
    free(particle_array);
    return 0;
}