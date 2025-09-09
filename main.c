#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "dbg.h"
#include "file.h"
#include "thread_op.h"
#include "cpu_thread.h"
#include "process.h"

Queue *readyQueue;
Queue *waitQueue;
process_t *running_pd;
int global_counter = 0;
int TOTAL_PROCESS  = 0;

sem_t empty;
sem_t full;
pthread_mutex_t g_counter_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readyQueue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t waitQueue_mutex = PTHREAD_MUTEX_INITIALIZER;


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
//         // printf("Initializing FCFS (First-Come-First-Serve) scheduler...\n");
//         init_fcfs();
//     } else if (strcmp(scheduler, "sjf") == 0) {
//         // printf("Initializing SJF (Shortest Job First) scheduler...\n");
//         init_sjf();
//     } else if (strcmp(scheduler, "priority") == 0) {
//         // printf("Initializing Priority scheduler (preemptive)...\n");
//         init_priority();
//     } else if (strcmp(scheduler, "rr") == 0) {
//         // printf("Initializing Round Robin scheduler (time quantum: 4)...\n");
//         init_rr();
//     } else {
//         printf("Error: Unknown scheduler '%s'\n\n", scheduler);
//         print_usage("./cpu_scheduler");
//         exit(1);
//     }
// }

int main(int argc, char *argv[]){
    // if (argc != 3) {
    //     print_usage(argv[0]);
    //     return 1;
    // }

    // char *scheduler = argv[1];
    char *input_file = argv[1];
    
    // Read the input file
    burst_data *data = read_burstfile(input_file);
    check(data != NULL, "Failed to read input file '%s'", input_file);

    init_queues();
    
    pthread_t arrivalThread;
    // pthread_t cpuThread;
    // pthread_t ioThread;

    sem_init(&empty, 0, data->t_process); 
    sem_init(&full, 0, 0);        

    // add_arrival_process(&data);
    if (pthread_create(&arrivalThread, NULL, (void *)add_arrival_process, &data) != 0) {
        perror("Failed to create arrival thread");
        return 1;
    }
    pthread_join(arrivalThread, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
error:
    return 1;
}