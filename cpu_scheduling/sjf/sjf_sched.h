#ifndef SJF_H
#define SJF_H

#include "file.h"
#include "sched_common.h"
#include "time_t.h"

extern process_t *running_pd;
extern process_t *sleeping_pd;
extern scheduler_ops_t *process_core;

extern timer__t *wall_timer;
extern int start_time, end_time;

void init_sjf();
void destroy_sjf();

void *schedular_sjf();
void *wakeUp_sjf();
void *add_arrival_process_sjf(burst_data **);

#endif