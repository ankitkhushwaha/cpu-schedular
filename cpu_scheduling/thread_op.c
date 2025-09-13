#include "thread_op.h"
#include "dbg.h"
#include "file.h"
#include "process.h"
#include "queue.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
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
    pthread_mutex_lock(&readyQueue_mutex);
    enqueue(readyQueue, p);
    pthread_mutex_unlock(&readyQueue_mutex);
    return NULL;
}

void *add_to_waitQueue(process_t *p) {
    pthread_mutex_lock(&waitQueue_mutex);
    enqueue(waitQueue, p);
    pthread_mutex_unlock(&waitQueue_mutex);
    return NULL;
}

void *add_to_taskList(process_t *p) {
    pthread_mutex_lock(&task_list_mutex);
    enqueue(task_list, p);
    pthread_mutex_unlock(&task_list_mutex);
    return NULL;
}

void *update_global_counter(int val) {
    // pthread_mutex_lock(&g_counter_mutex);
    atomic_fetch_add(&global_counter, val);
    // global_counter += val;
    // pthread_mutex_unlock(&g_counter_mutex);
    return NULL;
}

int read_global_counter() {
    // pthread_mutex_lock(&g_counter_mutex);
    int val = atomic_load(&global_counter);
    // pthread_mutex_unlock(&g_counter_mutex);
    return val;
}

bool update_term_counter(int val) {
    // pthread_mutex_lock(&term_counter_mutex);
    atomic_fetch_add(&TERMINATED_PROCESS, val);
    // TERMINATED_PROCESS += val;
    // pthread_mutex_unlock(&term_counter_mutex);
    return true;
}

int read_term_counter() {
    // pthread_mutex_lock(&term_counter_mutex);
    return atomic_load(&TERMINATED_PROCESS);
    // pthread_mutex_unlock(&term_counter_mutex);
}

bool is_emptyReadyQueue_t() {
    pthread_mutex_lock(&readyQueue_mutex);
    bool val = isEmpty(readyQueue);
    pthread_mutex_unlock(&readyQueue_mutex);
    return val;
}

bool is_emptyWaitQueue_t() {
    pthread_mutex_lock(&waitQueue_mutex);
    bool val = isEmpty(waitQueue);
    pthread_mutex_unlock(&waitQueue_mutex);
    return val;
}

bool is_emptyTaskList_t() {
    pthread_mutex_lock(&task_list_mutex);
    bool val = isEmpty(task_list);
    pthread_mutex_unlock(&task_list_mutex);
    return val;
}

process_t *remove_from_readyQueue() {
    pthread_mutex_lock(&readyQueue_mutex);
    process_t *pd = dequeue(readyQueue);
    pthread_mutex_unlock(&readyQueue_mutex);

    return pd;
}

process_t *remove_from_waitQueue() {
    pthread_mutex_lock(&waitQueue_mutex);
    process_t *pd = dequeue(waitQueue);
    pthread_mutex_unlock(&waitQueue_mutex);

    return pd;
}

process_t *remove_node_by_pid_t(Queue *queue, int pid, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    process_t *pd = remove_node_by_pid(queue, pid);
    pthread_mutex_unlock(mutex);
    return pd;
}

int read_wait_sem_value_t() {
    int tmp;
    sem_getvalue(&wait_count, &tmp);
    return tmp;
}

int read_ready_sem_value_t() {
    int tmp;
    sem_getvalue(&ready_count, &tmp);
    return tmp;
}

bool all_processes_done() {
    return (read_term_counter() >= TOTAL_PROCESS) && is_emptyReadyQueue_t() &&
           is_emptyWaitQueue_t();
}

void *write_cpu_process_data(process_t *pd, int start, int end) {
    write_process_data(task_log, pd->pid, pd->cpu_index, start, end);
    return NULL;
}

// NOT THREAD SAFE
process_t *find_process(Queue *queue, int pid) {
    node *tnode;
    Traverse(tnode, queue) {
        if (tnode->data->pid == pid) {
            return tnode->data;
        }
    }
    return NULL;
}
