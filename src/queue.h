#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include "file.h"

typedef struct Node
{
    burst_line *data;
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
node *enqueue(Queue *, burst_line *);
node *dequeue(Queue *);
bool isEmpty(Queue *);

#endif