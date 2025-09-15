#include "srtf_th.h"
#include "dbg.h"
#include "priority_queue.h"
#include "process.h"
#include "queue.h"
#include "thread_op.h"
#include <pthread.h>
#include <stdlib.h>

void init_srtf_th() {
    thread_core = create_thread_op();
    check_mem(thread_core);
    thread_core->add_to_readyQueue_core = add_to_readyQueue_srtf;
    thread_core->add_to_waitQueue_core = add_to_waitQueue_srtf;
    thread_core->add_to_taskList_core = add_to_taskList_srtf;
    thread_core->remove_from_readyQueue_core = remove_from_readyQueue_srtf;
    thread_core->remove_from_waitQueue_core = remove_from_waitQueue_srtf;
    thread_core->remove_node_by_pid_core = remove_node_by_pid_t_srtf;

    return;
error:
    exit(EXIT_FAILURE);
}

void *add_to_readyQueue_srtf(process_t *p) {
    pthread_mutex_lock(&readyQueue_mutex);
    p_enqueue(readyQueue, p, p->priority);
    pthread_mutex_unlock(&readyQueue_mutex);
    return NULL;
}

void *add_to_waitQueue_srtf(process_t *p) {
    pthread_mutex_lock(&waitQueue_mutex);
    p_enqueue(waitQueue, p, p->priority);
    pthread_mutex_unlock(&waitQueue_mutex);
    return NULL;
}

void *add_to_taskList_srtf(process_t *p) {
    pthread_mutex_lock(&task_list_mutex);
    p_enqueue(task_list, p, p->priority);
    pthread_mutex_unlock(&task_list_mutex);
    return NULL;
}

process_t *remove_from_readyQueue_srtf() {
    pthread_mutex_lock(&readyQueue_mutex);
    process_t *pd = p_dequeue(readyQueue);
    pthread_mutex_unlock(&readyQueue_mutex);

    return pd;
}

process_t *remove_from_waitQueue_srtf() {
    pthread_mutex_lock(&waitQueue_mutex);
    process_t *pd = p_dequeue(waitQueue);
    pthread_mutex_unlock(&waitQueue_mutex);

    return pd;
}

process_t *remove_node_by_pid_t_srtf(Queue *queue, int pid, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    process_t *pd = p_remove_node_by_pid(queue, pid);
    pthread_mutex_unlock(mutex);
    return pd;
}
