#ifndef CPU_THREAD_H
#define CPU_THREAD_H

#include "file.h"
#include "process.h"
#include "queue.h"
#include "thread_op.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>

extern int TOTAL_PROCESS;
extern atomic_int TERMINATED_PROCESS;

extern atomic_int wait_counter;
extern atomic_int ready_counter;

extern bool is_emptyWaitQueue_t();
extern bool is_emptyReadyQueue_t();

extern process_t *sleeping_pd;
extern process_t *running_pd;
extern sem_t wait_count;
extern sem_t ready_count;

void *schedular();
void *wake_up();
void *add_arrival_process(burst_data **);

#endif