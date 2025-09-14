#ifndef FILE_H
#define FILE_H

#include <stdio.h>

typedef struct burst_line {
    int *cpu_burst;
    int *io_burst;
    int a_time;
    int cpu_burst_size;
    int io_burst_size;
} burst_line;

typedef struct burst_data {
    burst_line **b_data;
    int t_process;
} burst_data;

burst_data *read_burstfile(char *);
void write_process_data(FILE *, int, int, int, int);
#endif