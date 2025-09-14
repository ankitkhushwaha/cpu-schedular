#include "dbg.h"
#include "file.h"
#include "process.h"
#include "queue.h"
#include "result.h"
#include "sched_common.h"
#include "thread_op.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

Queue *readyQueue;
Queue *waitQueue;
Queue *task_list;
atomic_int global_counter = 0;
int T_PROCESS = 0;
int TOTAL_PROCESS = 0;
atomic_int TERMINATED_PROCESS = 0;
FILE *task_log;

process_t *running_pd;
process_t *sleeping_pd;
scheduler_ops_t *process_core;
thread_op_t *thread_core;

extern void queue_print(Queue *queue);
extern void isValidQueue(Queue *queue);

sem_t wait_count;
sem_t ready_count;
pthread_mutex_t readyQueue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitQueue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t task_list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t term_counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void print_usage(const char *prog_name) {
    printf("Usage: %s <scheduler> <input_file>\n", prog_name);
    printf("Available schedulers:\n");
    printf("  fcfs    - First-Come-First-Serve scheduling\n");
    printf("  sjf     - Shortest Job First (non-preemptive)\n");
    printf("  priority - Priority scheduling\n");
    printf("  rr      - Round Robin\n");
}

int main(int argc, char *argv[]) {
    // if (argc != 3) {
    //     print_usage(argv[0]);
    //     return 1;
    // }

    // char *scheduler = argv[1];
    char *input_file = argv[1];

    task_log = fopen("task_log.txt", "w");
    if (!task_log) {
        perror("Failed to open task_log.txt");
        exit(EXIT_FAILURE);
    }
    fprintf(task_log, "CPU0\n");
    // Read the input file
    burst_data *data = read_burstfile(input_file);
    check(data != NULL, "Failed to read input file '%s'", input_file);
    TOTAL_PROCESS = data->t_process;
    init_queues();
    init_scheduler("sjf");
    pthread_t arrivalThread, schedularThread, wakeupThread;
    sem_init(&wait_count, 0, 0);
    sem_init(&ready_count, 0, 0);

    // add_arrival_process(&data);
    // pthread_create(&arrivalThread, NULL, (void *)add_arrival_process, &data);
    // we want to wait for arrival thread to finish before creating schedular thread
    if (pthread_create(&arrivalThread, NULL, (void *)process_core->add_arrival_core, &data) != 0) {
        perror("Failed to create arrival thread");
        return 1;
    }
    pthread_join(arrivalThread, NULL);

    if (pthread_create(&schedularThread, NULL, (void *)process_core->scheduler_core, NULL) != 0) {
        perror("Failed to create schedular thread");
        return 1;
    }

    if (pthread_create(&wakeupThread, NULL, (void *)process_core->wakeUp_core, NULL) != 0) {
        perror("Failed to create wake_up thread");
        return 1;
    }
    // schedular();
    // process_t *pd = remove_node_by_pid(readyQueue, 0);
    // isValidQueue(readyQueue);
    // process_t *pd2 = remove_node_by_pid(readyQueue, 2);
    // isValidQueue(readyQueue);
    // process_t *pd7 = remove_node_by_pid(readyQueue, 3);
    // isValidQueue(readyQueue);
    // process_t *pd6 = remove_node_by_pid(readyQueue, 6);
    // isValidQueue(readyQueue);

    // // queue_print(readyQueue);
    // process_t *pd4 = remove_node_by_pid(readyQueue, 4);
    // isValidQueue(readyQueue);
    // process_t *pd5 = remove_node_by_pid(readyQueue, 5);
    // isValidQueue(readyQueue);
    // process_t *pd1 = remove_node_by_pid(readyQueue, 1);
    // isValidQueue(readyQueue);

    pthread_join(wakeupThread, NULL);
    pthread_join(schedularThread, NULL);
    // wake_up();
    write_result();
    sem_destroy(&wait_count);
    sem_destroy(&ready_count);
    pthread_mutex_destroy(&readyQueue_mutex);
    pthread_mutex_destroy(&waitQueue_mutex);
    pthread_mutex_destroy(&task_list_mutex);
    pthread_mutex_destroy(&term_counter_mutex);

    fclose(task_log);
    return 0;
error:
    return 1;
}