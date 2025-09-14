#ifndef SJF_TH_H
#define SJF_TH_H

#include <pthread.h>
#include "thread_op.h"

extern Queue *readyQueue;
extern Queue *waitQueue;
extern Queue *task_list;

extern pthread_mutex_t readyQueue_mutex;
extern pthread_mutex_t waitQueue_mutex;
extern pthread_mutex_t task_list_mutex;

extern thread_op_t *thread_core;

void init_sjf_th();
void destroy_sjf_th();

void *add_to_readyQueue_sjf(process_t *p);
void *add_to_waitQueue_sjf(process_t *p);
void *add_to_taskList_sjf(process_t *p);

process_t *remove_from_readyQueue_sjf();
process_t *remove_from_waitQueue_sjf();
process_t *remove_node_by_pid_t_sjf(Queue *, int, pthread_mutex_t *);

#endif
