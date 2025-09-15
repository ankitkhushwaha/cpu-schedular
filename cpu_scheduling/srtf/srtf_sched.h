#ifndef SRTF_H
#define SRTF_H

#include "file.h"
#include "queue.h"
#include "sched_common.h"
#include "time_t.h"
#include <pthread.h>

extern Queue *readyQueue;
extern pthread_mutex_t readyQueue_mutex;

extern process_t *running_pd;
extern process_t *sleeping_pd;
extern scheduler_ops_t *process_core;

extern timer__t *wall_timer;
extern int start_time, end_time;

void init_srtf();
void destroy_srtf();

void *schedular_srtf();
void *wakeUp_srtf();
void *add_arrival_process_srtf(burst_data **);

#endif