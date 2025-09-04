#include <stdio.h>
#include "file.h"
#include "thread_op.h"
#include "queue.h"


void *init_queues(){
    readyQueue = queue_create();
    waitQueue = queue_create();
    return NULL;
}

void *add_to_readyQueue(burst_line *process_data){
    enqueue(readyQueue, process_data);
    return NULL;
}

void *add_to_waitQueue(burst_line *process_data){
    enqueue(waitQueue, process_data);
    return NULL;
}

node *remove_from_readyQueue(){
    return dequeue(readyQueue);
}

node *remove_from_waitQueue(){
    return dequeue(waitQueue);
}