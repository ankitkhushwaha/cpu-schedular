#include "process.h"
#include "queue.h"
#include "dbg.h"
#include "sjf_th.h"
#include <pthread.h>
#include "thread_op.h"

void init_fcfs_th(){
    thread_core = create_thread_op();
    check(process_core, "failed to create scheduler");
    thread_core->add_to_readyQueue_core = add_to_readyQueue_sjf;
    thread_core->add_to_waitQueue_core = add_to_waitQueue_sjf;
    thread_core->add_to_taskList_core = add_to_taskList_sjf;
    thread_core->remove_from_readyQueue_core = remove_from_readyQueue_sjf;
    thread_core->remove_from_waitQueue_core = remove_from_waitQueue_sjf;
    thread_core->remove_node_by_pid_core = remove_node_by_pid_t_sjf;

    return;
error:
    exit(EXIT_FAILURE);
}

void *add_to_readyQueue_sjf(process_t *p) {
    pthread_mutex_lock(&readyQueue_mutex);
    enqueue(readyQueue, p);
    pthread_mutex_unlock(&readyQueue_mutex);
    return NULL;
}

void *add_to_waitQueue_sjf(process_t *p) {
    pthread_mutex_lock(&waitQueue_mutex);
    enqueue(waitQueue, p);
    pthread_mutex_unlock(&waitQueue_mutex);
    return NULL;
}

void *add_to_taskList_sjf(process_t *p) {
    pthread_mutex_lock(&task_list_mutex);
    enqueue(task_list, p);
    pthread_mutex_unlock(&task_list_mutex);
    return NULL;
}

process_t *remove_from_readyQueue_sjf() {
    pthread_mutex_lock(&readyQueue_mutex);
    process_t *pd = dequeue(readyQueue);
    pthread_mutex_unlock(&readyQueue_mutex);

    return pd;
}

process_t *remove_from_waitQueue_sjf() {
    pthread_mutex_lock(&waitQueue_mutex);
    process_t *pd = dequeue(waitQueue);
    pthread_mutex_unlock(&waitQueue_mutex);

    return pd;
}

process_t *remove_node_by_pid_sjf(Queue *queue, int pid, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    process_t *pd = remove_node_by_pid(queue, pid);
    pthread_mutex_unlock(mutex);
    return pd;
}
