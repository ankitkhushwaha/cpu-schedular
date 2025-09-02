#ifndef FILE_H
#define FILE_H

#include <stdio.h>


typedef struct burst_line
{
    int *cpu_burst;
    int *io_burst;
    int a_time;
} burst_line;

burst_line *burst_data;

extern burst_data *read_burstfile(char *);

#endif