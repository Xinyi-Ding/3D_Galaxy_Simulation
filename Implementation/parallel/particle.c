#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#include <limits.h>
#include <stdlib.h>

#include "particle.h"
#define pi 3.14159265358979323846
#define GRAVITY 1.0
double drand_custom(double low, double high)
{
    double range = high - low;
    return ((double)rand() * range) / (double)RAND_MAX + low;
}

double clamp(double in)
{
    if (in >= 0.0)
    {
        double tmp = in < DBL_MIN ? DBL_MIN : in;
        return tmp > DBL_MAX ? DBL_MAX : tmp;
    }
    else
    {
        double tmp = in < -DBL_MAX ? -DBL_MAX : in;
        return tmp > -DBL_MIN ? -DBL_MIN : tmp;
    }
}
int get_octant(Particle particle, Space space)
{
    int octant = 0;
    if (particle.x > ((space.origin_x + space.boundary_x) / 2.0))
    {
        octant |= (1 << 0);
    }

    if (particle.y > ((space.origin_y + space.boundary_y) / 2.0))
    {
        octant |= (1 << 1);
    }

    if (particle.z > ((space.origin_z + space.boundary_z) / 2.0))
    {
        octant |= (1 << 2);
    }
    return octant;
}

void generate_random_particles(Particle *p, Space space, int count, float galaxy_thickness)
{
    double a = 1.0;
    for (int i = 0; i < count; i++)
    {
        // Generate spherical coordinates
        // double r = drand_custom(0.0, galaxy_diameter / 2.0f); // Radius up to half the side length of the cube (to fit within the cube)
        // double r = a*pow(drand_custom(0.0,1.0), 0.3333) / sqrt(1 - pow(drand_custom(0.0,1.0), 0.66667));
        double r = drand_custom(1.0,1000.0);


        double theta = drand_custom(0.0, 2.0 * pi);                                   // Theta angle from 0 to 2*PI
        double phi = drand_custom(0.0, pi);                                           // Phi angle from 0 to PI
        double z_pos = drand_custom(-galaxy_thickness / 2.0, galaxy_thickness / 2.0); // z position within thickness
        // set mass and position of particle
		if(i==0){
			p[i].x = 0.0;
            p[i].y = 0.0;
            p[i].z = 0.0;
            p[i].vel_x = 0.0;
            p[i].vel_y = 0.0;
            p[i].vel_z = 0.0;
            p[i].mass = 100000.0;
            p[i].id = i;
		}
        else
        {
            p[i].x = r * cos(theta);
            p[i].y = r * sin(theta);
            p[i].z = z_pos;

            // set velocity of particle
            float rotation = 1; // 1: clockwise   -1: counter-clockwise
            float v = 1.0 * sqrt(100000.0 / r);
            // float v = 0.5 * sqrt(100000.0 / r);
            p[i].vel_x = rotation * v * sin(theta);
            p[i].vel_y = -rotation * v * cos(theta);
            p[i].vel_z = 0.0;

            p[i].mass = 1.0; 
            p[i].id = i;
        }
    }
}

void print_particle(Particle p)
{
    printf("id: %d | location: (%f, %f, %f) | mass: %f | force: (%f, %f, %f)\n", p.id, p.x, p.y, p.z, p.mass, p.force_x, p.force_y, p.force_z);
}

void print_space(Space s)
{
    printf("origin: (%f, %f, %f) | boundary: (%f, %f, %f)\n", s.origin_x, s.origin_y, s.origin_z, s.boundary_x, s.boundary_y, s.boundary_z);
}

double compute_distance(Particle *particle, double com_x, double com_y, double com_z)
{
    double smoothing_length = 1.0;
    return sqrt(clamp(pow(particle->x - com_x, 2)) + clamp(pow(particle->y - com_y, 2)) + clamp(pow(particle->z - com_z, 2)) + smoothing_length);
}

void update_particle_position_and_velocity(Particle *p)
{
    double dt = 0.1;
    double acc_x = clamp(p->force_x / p->mass);
    double acc_y = clamp(p->force_y / p->mass);
    double acc_z = clamp(p->force_z / p->mass);

    // Compute middle of timestep velocity.
    double v_mid_x = p->vel_x + clamp(0.5 * dt * acc_x);
    double v_mid_y = p->vel_y + clamp(0.5 * dt * acc_y);
    double v_mid_z = p->vel_z + clamp(0.5 * dt * acc_z);

    // Compute new position.
    double prev = p->x;
    p->x = p->x + clamp(dt * v_mid_x);
    p->y = p->y + clamp(dt * v_mid_y);
    p->z = p->z + clamp(dt * v_mid_z);

    // Compute second half of velocity.
    p->vel_x = v_mid_x + clamp(0.5 * dt * acc_x);
    p->vel_y = v_mid_y + clamp(0.5 * dt * acc_y);
    p->vel_z = v_mid_z + clamp(0.5 * dt * acc_z);
}
bool in_space(Space s, Particle p)
{
    return (p.x > s.origin_x) && (p.x < s.boundary_x) && (p.y > s.origin_y) && (p.y < s.boundary_y) && (p.z > s.origin_z) && (p.z < s.boundary_z);
}
