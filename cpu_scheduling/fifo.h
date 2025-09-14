#ifndef FIFO_H
#define FIFO_H

#include "sched_common.h"
#include "file.h"
#include "time_t.h"

extern process_t *running_pd;
extern process_t *sleeping_pd; 
extern scheduler_ops_t *process_core;

extern timer__t *wall_timer;
extern int start_time, end_time;

void init_fcfs();
void destroy_fcfs();

void *schedular_fifo();
void *wakeUp_fifo();
void *add_arrival_process_fifo(burst_data **);

#endif