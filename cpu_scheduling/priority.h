#ifndef PRIORITY_H
#define PRIORITY_H

#include "queue.h"
#include <pthread.h>

// External variables
extern Queue *readyQueue;
extern Queue *waitQueue;
extern pthread_mutex_t mutex;

// Function declarations
void init_priority();
void *priority_scheduler(void *arg);

// Priority scheduling can be preemptive or non-preemptive
#define PREEMPTIVE 1  // Set to 0 for non-preemptive

typedef struct burst_line {
    int p_id;           // Process ID
    int a_time;         // Arrival time
    int b_time;         // CPU burst time
    int priority;       // Process priority (lower number = higher priority)
    int io_burst;       // I/O burst time
    int remaining_time; // Remaining CPU burst time
    int last_executed;  // Last time the process was executed
    int response_time;  // Response time (first time process gets CPU)
} burst_line;

// Comparison function for priority scheduling
int compare_priority(const void *a, const void *b);

#endif
