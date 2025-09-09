#include <stdio.h>
#include <stdlib.h>
#include "process.h"
#include "dbg.h"
#include "queue.h"
#include "process.h"

Queue *queue_create()
{
    Queue *queue = (Queue *)calloc(1, sizeof(Queue));
    queue->len = 0;
    check_mem(queue);
    return queue;
error:
    exit(EXIT_FAILURE);
}

node *enqueue(Queue *queue, process_t *pd)
{
    node *newnode = (node *)calloc(1, sizeof(node));
    check_mem(newnode);

    newnode->data = (process_t *)calloc(1, sizeof(process_t));
    check_mem(newnode->data);
    newnode->data = pd;
    check_mem(newnode);

    if (!queue->front)
    {
        queue->front = newnode;
        queue->len++;
        return queue->front;
    }
    node *tnode;
    Traverse(tnode, queue);
    tnode->next = newnode;
    queue->rear = tnode->next;

    queue->len++;
    return queue->front;
error:
    return NULL;
}

process_t *dequeue(Queue *queue)
{

    if (isEmpty(queue))
    {
        printf("queue is empty\n");
        return NULL;
    }
    if (!queue->front->next)
    {        
        node *tnode = queue->front;
        queue->front = queue->rear = NULL;
        queue->len--;
        free(tnode);
        return NULL;
    }

    node *tnode, *snode;
    tnode = snode = queue->front;
    snode->next = NULL;

    tnode = tnode->next;
    queue->front = tnode;
    queue->len--;

    return snode->data;
}

bool isEmpty(Queue *queue)
{
    if (queue->len == 0)
        return true;
    else
        return false;
}

void queue_print(Queue *queue)
{
    if (isEmpty(queue))
    {
        printf("queue is empty\n");
        return;
    }
    node *tnode;
    Traverse(tnode, queue)
    {
        // printf("%d ", tnode->data);
    }
    // printf("%d\n", tnode->data);
}

// int main(int argc, char *argv[])
// {
//     Queue *queue = queue_create();
//     enqueue(queue, 2);
//     enqueue(queue, 3);
//     enqueue(queue, 4);

//     // int arr[] = {2, 3, 4};
//     // validate_list(list, arr, 3);

//     queue_print(queue);

//     dequeue(queue);
//     dequeue(queue);
//     dequeue(queue);
//     queue_print(queue);
    // dequeue(queue);
    // list_RemoveFromEnd(list);
    // list_print(list);
    // list_add(list, 5);

    // int arr2[] = {2, 3, 5};
    // validate_list(list, arr2, 3);

    // list_RemoveFromFront(list);
    // list_add(list, 1);

    // int arr3[] = {3, 5, 1};
    // validate_list(list, arr3, 3);

    // list_print(list);
// }