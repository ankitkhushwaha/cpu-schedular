#ifndef RR_SCHED_H
#define RR_SCHED_H

#include "file.h"
#include "sched_common.h"
#include "time_t.h"
#include <pthread.h>

extern process_t *running_pd;
extern process_t *sleeping_pd;
extern scheduler_ops_t *process_core;

extern pthread_mutex_t task_list_mutex;

extern timer__t *wall_timer;
extern int start_time, end_time, quantum;

void init_rr();
void destroy_rr();

void *schedular_rr();
void *wakeUp_rr();
void *add_arrival_process_rr(burst_data **);

#endif