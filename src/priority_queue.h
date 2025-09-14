#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "queue.h"

Queue *p_queue_create();
node *p_enqueue(Queue *, process_t *);
node *p_newNode(process_t *pd, int priority);
process_t *p_dequeue(Queue *);
bool p_isEmpty(Queue *);
process_t *p_remove_node_by_pid(Queue *, int);
void p_isValidQueue(Queue *);
void p_queue_print(Queue *);
#endif
