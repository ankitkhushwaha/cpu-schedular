#ifndef FIFO_H
#define FIFO_H

#include "sched_common.h"

extern scheduler_ops_t *process_core;

void init_fcfs();
void destroy_fcfs();

void *schedular_fifo();
void *wakeUp_fifo();

#endif