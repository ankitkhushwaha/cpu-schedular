#ifndef RR_TH_H
#define RR_TH_H

#include "thread_op.h"
#include <pthread.h>

extern Queue *readyQueue;
extern Queue *waitQueue;
extern Queue *task_list;

extern pthread_mutex_t readyQueue_mutex;
extern pthread_mutex_t waitQueue_mutex;
extern pthread_mutex_t task_list_mutex;

extern thread_op_t *thread_core;

void init_rr_th();
void destroy_rr_th();

void *add_to_readyQueue_rr(process_t *p);
void *add_to_waitQueue_rr(process_t *p);
void *add_to_taskList_rr(process_t *p);

process_t *remove_from_readyQueue_rr();
process_t *remove_from_waitQueue_rr();
process_t *remove_node_by_pid_t_rr(Queue *, int, pthread_mutex_t *);

#endif
