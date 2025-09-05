#ifndef THREAD_OP_H
#define THREAD_OP_H

#include "queue.h"
#include "process.h"

extern Queue *readyQueue;
extern Queue *waitQueue;

void *init_queues();
void *add_to_readyQueue(burst_line *);
void *add_to_waitQueue(burst_line *);
process_t *remove_from_readyQueue();
process_t *remove_from_waitQueue();

#endif