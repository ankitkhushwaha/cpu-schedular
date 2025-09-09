#include <stdio.h>
#include <stdlib.h>
#include "dbg.h"
#include "file.h"
#include "thread_op.h"
#include "queue.h"
#include "process.h"

void *init_queues()
{
    readyQueue = queue_create();
    check(readyQueue != NULL, "readyQueue failed to create\n");
    waitQueue = queue_create();
    check(waitQueue != NULL, "waitQueue failed to create\n");
    return NULL;
error:
    exit(EXIT_FAILURE);
}

void *add_to_readyQueue(burst_line *process_data)
{
    process_t *p = create_process();
    check(p, "failed to create process");
    p->process_d = process_data;
    enqueue(readyQueue, p);
    return NULL;
error:
    perror("Failed to create process");
    return NULL;
}

void *add_to_waitQueue(burst_line *process_data)
{
    process_t *p = create_process();
    check(p, "failed to create process");
    p->process_d = process_data;
    enqueue(waitQueue, p);
    return NULL;
error:
    perror("Failed to create process");
    return NULL;
}

process_t *remove_from_readyQueue()
{
    return dequeue(readyQueue);
}

process_t *remove_from_waitQueue()
{
    return dequeue(waitQueue);
}