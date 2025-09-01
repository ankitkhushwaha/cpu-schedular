#ifndef QUEUE_H
#define QUEUE_H

#include "linkedList.h"

typedef struct
{
    struct Node *front;
    struct Node *rear;
} queue;

queue *queue_create();
node *enqueue(queue *);
node *dequeue(queue *);
bool isEmpty(queue *);

#endif