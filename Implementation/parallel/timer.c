#include "timer.h"
#include <time.h>

static struct timespec start_time;

void StartTimer() {
    clock_gettime(CLOCK_MONOTONIC, &start_time);
}

double GetTimer() {
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double start = start_time.tv_sec + start_time.tv_nsec * 1e-9;
    double end = end_time.tv_sec + end_time.tv_nsec * 1e-9;
    return end - start;
}