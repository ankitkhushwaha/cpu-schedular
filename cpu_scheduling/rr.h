#ifndef RR_H
#define RR_H

#include "queue.h"
#include <pthread.h>

// External variables
extern Queue *readyQueue;
extern Queue *waitQueue;
extern pthread_mutex_t mutex;

// Time quantum for Round Robin (in time units)
#define TIME_QUANTUM 4

// Function declarations
void init_rr();
void *rr_scheduler(void *arg);

extern int global_counter;

typedef struct burst_line {
    int p_id;           // Process ID
    int a_time;         // Arrival time
    int b_time;         // CPU burst time
    int priority;       // Process priority
    int io_burst;       // I/O burst time
    int remaining_time; // Remaining CPU burst time (important for RR)
    int last_executed;  // Last time the process was executed
} burst_line;

#endif
