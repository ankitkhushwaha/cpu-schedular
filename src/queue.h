#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>

typedef struct {
    process_t *data;
    int priority;
    node *next;
} node;

typedef struct {
    int len;
    node *front;
    node *rear;
} Queue;

#define Traverse(tnode, queue) for (tnode = queue->front; tnode->next != NULL; tnode = tnode->next)

Queue *queue_create();
node *enqueue(Queue *, process_t *);
process_t *dequeue(Queue *);
bool isEmpty(Queue *);
process_t *remove_node_by_pid(Queue *, int);
void isValidQueue(Queue *);
#endif