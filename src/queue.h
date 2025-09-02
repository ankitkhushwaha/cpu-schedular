#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

typedef struct Node
{
    int data;
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
node *enqueue(Queue *, int);
node *dequeue(Queue *);
bool isEmpty(Queue *);

#endif