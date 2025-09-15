#ifndef SRTF_TH_H
#define SRTF_TH_H

#include "thread_op.h"
#include <pthread.h>

extern Queue *readyQueue;
extern Queue *waitQueue;
extern Queue *task_list;

extern pthread_mutex_t readyQueue_mutex;
extern pthread_mutex_t waitQueue_mutex;
extern pthread_mutex_t task_list_mutex;

extern thread_op_t *thread_core;

void init_srtf_th();
void destroy_srtf_th();

void *add_to_readyQueue_srtf(process_t *p);
void *add_to_waitQueue_srtf(process_t *p);
void *add_to_taskList_srtf(process_t *p);

process_t *remove_from_readyQueue_srtf();
process_t *remove_from_waitQueue_srtf();
process_t *remove_node_by_pid_t_srtf(Queue *, int, pthread_mutex_t *);

#endif
