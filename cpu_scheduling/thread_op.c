#include "thread_op.h"
#include "dbg.h"
#include "file.h"
#include "process.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void *init_queues() {
    readyQueue = queue_create();
    check(readyQueue != NULL, "readyQueue failed to create\n");
    waitQueue = queue_create();
    check(waitQueue != NULL, "waitQueue failed to create\n");
    task_list = queue_create();
    check(task_list != NULL, "task_list failed to create\n");
    return NULL;
error:
    exit(EXIT_FAILURE);
}

void *add_to_readyQueue(process_t *p) {
    enqueue(readyQueue, p);
    return NULL;
}

void *add_to_waitQueue(process_t *p) {
    enqueue(waitQueue, p);
    return NULL;
}

void *add_to_taskList(process_t *p) {
    enqueue(task_list, p);
    return NULL;
}

process_t *remove_from_readyQueue() { return dequeue(readyQueue); }

process_t *remove_from_waitQueue() { return dequeue(waitQueue); }