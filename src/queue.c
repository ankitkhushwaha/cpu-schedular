#include "queue.h"
#include "dbg.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>

Queue *queue_create() {
    Queue *queue = (Queue *)calloc(1, sizeof(Queue));
    queue->len = 0;
    check_mem(queue);
    return queue;
error:
    exit(EXIT_FAILURE);
}

node *enqueue(Queue *queue, process_t *pd) {
    node *newnode = (node *)calloc(1, sizeof(node));
    check_mem(newnode);

    newnode->data = (process_t *)calloc(1, sizeof(process_t));
    check_mem(newnode->data);
    newnode->data = pd;
    check_mem(newnode);

    if (!queue->front) {
        queue->front = queue->rear = newnode;
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

process_t *dequeue(Queue *queue) {

    if (isEmpty(queue)) {
        debug("queue is empty");
        return NULL;
    }
    // Only 1 element
    if (queue->front == queue->rear) {
        node *tnode = queue->front;
        queue->front = queue->rear = NULL;
        queue->len--;
        free(tnode->next);
        return tnode->data;
    }
    // Only 2 elements
    if (queue->front->next == queue->rear) {
        node *tnode = queue->rear;
        queue->rear = queue->front;
        queue->len--;
        free(tnode->next);
        return tnode->data;
    }
    node *tnode, *snode;
    tnode = snode = queue->front;

    tnode = tnode->next;
    snode->next = NULL;
    queue->front = tnode;
    queue->len--;

    return snode->data;
}

void *remove_node_by_pid(Queue *queue, int pid) {
    if (isEmpty(queue)) {
        debug("queue is empty");
        return NULL;
    }
    // Only 1 element
    if (queue->front == queue->rear) {
        if (queue->front->data->pid == pid) {
            node *tnode = queue->front;
            queue->front = queue->rear = NULL;
            queue->len--;
            free(tnode->next);
            return tnode->data;
        }
    }
    node *tnode;
    Traverse(tnode, queue) {
        if (tnode->next->data->pid == pid) {
            node *snode = tnode->next;
            tnode->next = snode->next;
            queue->len--;
            free(snode);
            return NULL;
        }
    }
    debug("process %d not found", pid);
    return NULL;
}

bool isEmpty(Queue *queue) {
    if (queue->len == 0)
        return true;
    else
        return false;
}

void queue_print(Queue *queue) {
    if (isEmpty(queue)) {
        debug("queue is empty");
        return;
    }
    node *tnode;
    Traverse(tnode, queue) {
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