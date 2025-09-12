#include "cpu_thread.h"
#include "dbg.h"
#include "file.h"
#include "process.h"
#include "queue.h"
#include "thread_op.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>

Queue *readyQueue;
Queue *waitQueue;
Queue *task_list;
process_t *running_pd;
atomic_int global_counter = 0;
int T_PROCESS = 0;
int TOTAL_PROCESS = 0;
atomic_int TERMINATED_PROCESS = 0;

process_t *sleeping_pd;

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

// void init_scheduler(const char *scheduler) {
//     if (strcmp(scheduler, "fcfs") == 0) {
//         // printf("Initializing FCFS (First-Come-First-Serve)
//         scheduler...\n"); init_fcfs();
//     } else if (strcmp(scheduler, "sjf") == 0) {
//         // printf("Initializing SJF (Shortest Job First) scheduler...\n");
//         init_sjf();
//     } else if (strcmp(scheduler, "priority") == 0) {
//         // printf("Initializing Priority scheduler (preemptive)...\n");
//         init_priority();
//     } else if (strcmp(scheduler, "rr") == 0) {
//         // printf("Initializing Round Robin scheduler (time quantum:
//         4)...\n"); init_rr();
//     } else {
//         printf("Error: Unknown scheduler '%s'\n\n", scheduler);
//         print_usage("./cpu_scheduler");
//         exit(1);
//     }
// }

int main(int argc, char *argv[]) {
    // if (argc != 3) {
    //     print_usage(argv[0]);
    //     return 1;
    // }

    // char *scheduler = argv[1];
    char *input_file = argv[1];

    // Read the input file
    burst_data *data = read_burstfile(input_file);
    check(data != NULL, "Failed to read input file '%s'", input_file);
    TOTAL_PROCESS = data->t_process;
    init_queues();

    pthread_t arrivalThread, schedularThread, wakeupThread;
    sem_init(&wait_count, 0, 0);
    sem_init(&ready_count, 0, 0);

    // add_arrival_process(&data);
    // pthread_create(&arrivalThread, NULL, (void *)add_arrival_process, &data);
    if (pthread_create(&arrivalThread, NULL, (void *)add_arrival_process, &data) != 0) {
        perror("Failed to create arrival thread");
        return 1;
    }

    if (pthread_create(&schedularThread, NULL, (void *)schedular, NULL) != 0) {
        perror("Failed to create schedular thread");
        return 1;
    }

    if (pthread_create(&wakeupThread, NULL, (void *)wake_up, NULL) != 0) {
        perror("Failed to create wake_up thread");
        return 1;
    }
    pthread_join(arrivalThread, NULL);
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

    sem_destroy(&wait_count);
    sem_destroy(&ready_count);
    pthread_mutex_destroy(&readyQueue_mutex);
    pthread_mutex_destroy(&waitQueue_mutex);
    pthread_mutex_destroy(&task_list_mutex);
    pthread_mutex_destroy(&term_counter_mutex);

    return 0;
error:
    return 1;
}