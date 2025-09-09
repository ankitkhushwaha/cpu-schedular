#ifndef THREAD_OP_H
#define THREAD_OP_H

#include "queue.h"
#include "process.h"

extern Queue *readyQueue;
extern Queue *waitQueue;
extern Queue *task_list;

void *init_queues();
void *add_to_readyQueue(process_t *);
void *add_to_waitQueue(process_t *);
void *add_to_taskList(process_t *p);

process_t *remove_from_readyQueue();
process_t *remove_from_waitQueue();

void *process_pd(process_t *);
#endif