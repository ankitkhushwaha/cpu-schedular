#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include "file.h"

extern int TOTAL_PROCESS;

typedef struct
{
    int pid;
    burst_line *process_d;
    bool cpu_use;
    bool io_use;
    // 0 = terminated, < 0 = sleep, > 0 = running 
    int status;
    int cpu_time;
    int io_time;
} process_t;

process_t *create_process();

#endif