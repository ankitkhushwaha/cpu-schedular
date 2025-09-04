#ifndef THREAD_OP_H
#define THREAD_OP_H

#include "queue.h"

extern Queue *readyQueue;
extern Queue *waitQueue;

void *init_queues();
void *add_to_readyQueue(Queue *);
void *add_to_waitQueue(Queue *);
Queue *remove_from_readyQueue(Queue *);
Queue *remove_from_waitQueue(Queue *);

#endif