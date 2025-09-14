#include "queue.h"
#include "dbg.h"
#include "process.h"
#include <assert.h>
#include <stdbool.h>
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
    isValidQueue(queue);
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
    isValidQueue(queue);
    if (isEmpty(queue)) {
        debug("queue is empty");
        return NULL;
    }
    // Only 1 element
    if (queue->front == queue->rear) {
        node *tnode = queue->front;
        queue->front = queue->rear = NULL;
        queue->len--;
        process_t *data = tnode->data;
        free(tnode);
        return data;
    }
    // Only 2 elements
    if (queue->front->next == queue->rear) {
        node *tnode = queue->rear;
        queue->rear = queue->front;
        queue->front->next = NULL;
        queue->rear->next = NULL;

        queue->len--;
        process_t *data = tnode->data;
        free(tnode);
        return data;
    }
    node *tnode, *snode;
    tnode = snode = queue->front;

    tnode = tnode->next;
    snode->next = NULL;
    queue->front = tnode;
    queue->len--;

    process_t *data = snode->data;
    free(snode);
    return data;
}

process_t *remove_node_by_pid(Queue *queue, int pid) {
    isValidQueue(queue);
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
            process_t *data = tnode->data;
            free(tnode);
            return data;
        }
        return NULL;
    }
    // if 1st element is matched
    if (queue->front->data->pid == pid) {
        node *tnode = queue->front;
        queue->front = queue->front->next;
        queue->len--;
        process_t *data = tnode->data;
        free(tnode);
        return data;
    }
    if (queue->rear->data->pid == pid) {
        process_t *data = dequeue(queue);
        return data;
    }
    node *prev_node;
    Traverse(prev_node, queue) {
        if (prev_node->next->data->pid == pid) {
            node *mid_node, *tnode;
            mid_node = tnode = prev_node->next;
            prev_node->next = mid_node->next;
            queue->len--;
            process_t *data = tnode->data;
            free(tnode);
            return data;
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
    isValidQueue(queue);
    if (isEmpty(queue)) {
        debug("queue is empty");
        return;
    }
    node *tnode;
    Traverse(tnode, queue) { printf("%d ", tnode->data->pid); }
    printf("%d\n", tnode->data->pid);
}

void isValidQueue(Queue *queue) {
    if (queue->len == 0) {
        assert(queue->front == NULL && queue->rear == NULL);
        return;
    }

    if (queue->len == 1) {
        assert(queue->front == queue->rear);
        return;
    } else {
        int len = 1;
        node *tnode, *snode;
        snode = queue->front->next;
        Traverse(tnode, queue) {
            assert(tnode != NULL);
            assert(tnode->next == snode);
            assert(tnode->next != tnode);
            if (tnode != queue->rear)
                snode = snode->next;
            len++;
        }
        assert(tnode == queue->rear);
        debug("len: %d, queue->len: %d", len, queue->len);
        assert(len == queue->len);
    }
    debug("Queue is valid");
}
