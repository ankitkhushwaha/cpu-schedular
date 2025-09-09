#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "process.h"

typedef struct Node
{
    process_t *data;
    struct Node *next;
} node;

typedef struct
{
    int len;
    struct Node *front;
    struct Node *rear;
} Queue;

#define Traverse(tnode, queue) \
    for (tnode = queue->front; \
         tnode->next != NULL; \
         tnode = tnode->next)

Queue *queue_create();
node *enqueue(Queue *, process_t *);
process_t *dequeue(Queue *);
bool isEmpty(Queue *);

#endif