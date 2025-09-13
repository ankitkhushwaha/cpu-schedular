#ifndef PROCESS_H
#define PROCESS_H

#include "file.h"
#include <stdbool.h>

extern int TOTAL_PROCESS;
extern int T_PROCESS;

typedef enum { NEW, READY, SLEEP, RUNNIG, TERMINATED, UNDEFINED } STATUS;
extern const char *STATUS_ARR[7];

typedef struct {
    int pid;
    burst_line *process_d;
    int cpu_index;
    int io_index;
    STATUS status;
    int cpu_time;
    int io_time;
    int a_time;
    int process_time; // total cpu usage time
    // code is creating multiple threads that get scheduled by actual cpu
    // so we will use turnaround_time theortical formula to get it.
    int turnaround_time;
} process_t;

process_t *create_process();

// typedef struct burst_line
// {
//     int *cpu_burst;
//     int *io_burst;
//     int a_time;
//     int cpu_burst_size;
//     int io_burst_size;
// } burst_line;

// typedef struct burst_data
// {
//     burst_line **b_data;
//     int t_process;
// } burst_data;

#endif