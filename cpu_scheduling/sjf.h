#ifndef SJF_H
#define SJF_H

#include "queue.h"
#include <pthread.h>

// External variables
extern Queue *readyQueue;
extern Queue *waitQueue;
extern pthread_mutex_t mutex;

// Function declarations
void init_sjf();
void *sjf_scheduler(void *arg);

extern int global_counter;

typedef struct burst_line {
    int p_id;           // Process ID
    int a_time;         // Arrival time
    int b_time;         // CPU burst time
    int priority;       // Process priority
    int io_burst;       // I/O burst time
    int remaining_time; // Remaining CPU burst time
    int start_time;     // Time when process started execution
    int response_time;  // Response time (first time process gets CPU)
} burst_line;

// Comparison function for SJF (sorts by burst time, then by arrival time)
int compare_burst_time(const void *a, const void *b);

#endif
