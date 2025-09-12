#ifndef THREAD_OP_H
#define THREAD_OP_H

#include "process.h"
#include "queue.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdbool.h>

extern sem_t wait_count;
extern sem_t ready_count;

extern Queue *readyQueue;
extern Queue *waitQueue;
extern Queue *task_list;
extern atomic_int global_counter;
extern atomic_int TERMINATED_PROCESS;

extern pthread_mutex_t readyQueue_mutex;
extern pthread_mutex_t waitQueue_mutex;
extern pthread_mutex_t task_list_mutex;
extern pthread_mutex_t term_counter_mutex;

void *init_queues();
void *add_to_readyQueue(process_t *);
void *add_to_waitQueue(process_t *);
void *add_to_taskList(process_t *p);

void *update_global_counter(int);
int read_global_counter();

bool update_term_counter(int);
int read_term_counter();

bool is_emptyReadyQueue_t();
bool is_emptyWaitQueue_t();
bool is_emptyTaskList_t();

process_t *remove_from_readyQueue();
process_t *remove_from_waitQueue();
process_t *remove_node_by_pid_t(Queue *, int, pthread_mutex_t *);

int read_wait_sem_value_t();
int read_ready_sem_value_t();
bool all_processes_done();

void *process_pd(process_t *);
process_t *find_process(Queue *, int);
#endif