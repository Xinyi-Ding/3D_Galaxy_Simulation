#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define SOFTENING 1e-9f
#define pi 3.14159265358979323846
typedef struct { float x, y, z, vx, vy, vz; } Body;

float random_float(float min, float max) {
  return min + (float)rand() / ((float)RAND_MAX / (max - min));
}

void create_galaxy(Body *bodies, int n, float galaxy_diameter, float galaxy_thickness) {
  for (int i = 0; i < n; i++) {
    float r = random_float(0.0f, galaxy_diameter / 2.0f);
    float theta = random_float(0.0f, 2.0f * pi);
    float phi = random_float(0.0f, pi);

    bodies[i].x = r * sin(phi) * cos(theta);
    bodies[i].y = r * sin(phi) * sin(theta);
    bodies[i].z = r * cos(phi);
    bodies[i].z *= galaxy_thickness / galaxy_diameter;
    
    // Calculate the angle for velocity direction
    float velocity_theta = atan2(bodies[i].y, bodies[i].x);
    // Set rotational velocities to simulate circular motion
    float rotational_velocity = 2.0f;  // This can be adjusted to change the rotation speed
    bodies[i].vx = -rotational_velocity * sin(velocity_theta);
    bodies[i].vy = rotational_velocity * cos(velocity_theta);
    bodies[i].vz = 0.0f;
  }
}

void bodyForce(Body *p, float dt, int n, float smoothing_length) {
  #pragma omp parallel for schedule(dynamic)
  for (int i = 0; i < n; i++) { 
    float Fx = 0.0f; 
    float Fy = 0.0f; 
    float Fz = 0.0f;

    for (int j = 0; j < n; j++) {
      if (i != j) {
        float dx = p[j].x - p[i].x;
        float dy = p[j].y - p[i].y;
        float dz = p[j].z - p[i].z;
        float distSqr = dx*dx + dy*dy + dz*dz + smoothing_length;
        float dist = sqrtf(distSqr);
        float factor = 1.0f / (dist * distSqr);
        // float  factor = 1.0f / (dist * dist * dist + smoothing_length);

        Fx += dx * factor;
        Fy += dy * factor;
        Fz += dz * factor;
      }
    }

    p[i].vx += dt * Fx; 
    p[i].vy += dt * Fy; 
    p[i].vz += dt * Fz;
  }
}

void writePositions(const char* filename, Body* p, int nBodies) {
    FILE* file = fopen(filename, "w");
    for (int i = 0; i < nBodies; i++) {
        fprintf(file, "%f %f %f\n", p[i].x, p[i].y, p[i].z);
    }
    fclose(file);
}

int main(const int argc, const char** argv) {
  int nBodies = 50000;
  if (argc > 1) nBodies = atoi(argv[1]);

  const float dt = 0.01f; // time step
  const int nIters = 500;  // simulation iterations
  const float galaxy_diameter = 100.0f;
  const float galaxy_thickness = 5.0f;
  const float smoothing_length = 1.0f;
  const float stars_speed = 1.0f;

  int bytes = nBodies * sizeof(Body);
  float *buf = (float*)malloc(bytes);
  Body *p = (Body*)buf;

  create_galaxy(p, nBodies, galaxy_diameter, galaxy_thickness);

  double totalTime = 0.0;

  for (int iter = 1; iter <= nIters; iter++) {
    StartTimer();
    bodyForce(p, dt, nBodies, smoothing_length); // compute interbody forces

    for (int i = 0; i < nBodies; i++) { // integrate position
        p[i].x += p[i].vx * dt;
        p[i].y += p[i].vy * dt;
        p[i].z += p[i].vz * dt;
    }

    char filename[256];
    snprintf(filename, sizeof(filename), "positions_%d.txt", iter);
    writePositions(filename, p, nBodies);

    const double tElapsed = GetTimer() / 1000.0;
    if (iter > 1) { // First iter is warm up
      totalTime += tElapsed;
    }
#ifndef SHMOO
    printf("Iteration %d: %.3f seconds\n", iter, tElapsed);
#endif
  }
  double avgTime = totalTime / (double)(nIters - 1);

#ifdef SHMOO
  printf("%d, %0.3f\n", nBodies, 1e-9 * nBodies * nBodies / avgTime);
#else
  printf("Average rate for iterations 2 through %d: %.3f seconds per iteration.\n", nIters, avgTime);
  printf("%d Bodies: average %0.3f Billion Interactions / second\n", nBodies, 1e-9 * nBodies * nBodies / avgTime);
#endif
  free(buf);
  return 0;
}