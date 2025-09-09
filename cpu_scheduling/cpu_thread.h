#ifndef CPU_THREAD_H
#define CPU_THREAD_H

#include <pthread.h>
#include <semaphore.h>
#include "queue.h"
#include "file.h"
#include "process.h"
#include "thread_op.h"

extern STATUS;
extern int global_counter;
extern int TOTAL_PROCESS;
extern int TERMINATED_PROCESS;

extern process_t *running_pd;
extern sem_t empty;
extern sem_t full;
extern pthread_mutex_t g_counter_mutex;
extern pthread_mutex_t readyQueue_mutex;
extern pthread_mutex_t waitQueue_mutex;

void *schedular();
void *wake_up();
void *add_arrival_process(burst_data **);

#endif