#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "file.h"
#include "thread_op.h"
#include "cpu_thread.h"
#include "cpu_scheduling/fifo.h"
#include "cpu_scheduling/rr.h"
#include "cpu_scheduling/sjf.h"
#include "cpu_scheduling/priority.h"

// Global queue variables
Queue *readyQueue = NULL;
Queue *waitQueue = NULL;

// Global counter for tracking time
int global_counter = 0;

// Function prototypes
extern burst_data *read_burstfile(char *);
extern void *init_queues();
extern void arrivalQueue(burst_data *process_t);

// Semaphores and mutex
sem_t empty;
sem_t full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to print usage instructions
void print_usage(const char *prog_name) {
    printf("Usage: %s <scheduler> <input_file>\n", prog_name);
    printf("Available schedulers:\n");
    printf("  fcfs    - First-Come-First-Serve scheduling\n");
    printf("  sjf     - Shortest Job First (non-preemptive)\n");
    printf("  priority - Priority scheduling\n");
    printf("  rr      - Round Robin\n");
}

// Function to initialize the selected scheduler
void init_scheduler(const char *scheduler) {
    if (strcmp(scheduler, "fcfs") == 0) {
        printf("Initializing FCFS (First-Come-First-Serve) scheduler...\n");
        init_fcfs();
    } else if (strcmp(scheduler, "sjf") == 0) {
        printf("Initializing SJF (Shortest Job First) scheduler...\n");
        init_sjf();
    } else if (strcmp(scheduler, "priority") == 0) {
        printf("Initializing Priority scheduler (preemptive)...\n");
        init_priority();
    } else if (strcmp(scheduler, "rr") == 0) {
        printf("Initializing Round Robin scheduler (time quantum: 4)...\n");
        init_rr();
    } else {
        printf("Error: Unknown scheduler '%s'\n\n", scheduler);
        print_usage("./cpu_scheduler");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc != 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    char *scheduler = argv[1];
    char *input_file = argv[2];
    
    // Read the input file
    burst_data *data = read_burstfile(input_file);
    if (data == NULL) {
        fprintf(stderr, "Error: Failed to read input file '%s'\n", input_file);
        return 1;
    }
    
    printf("CPU Scheduler: %s\n", scheduler);
    printf("Number of processes: %d\n", data->t_process);
    
    // Initialize queues
    init_queues();
    
    // Initialize semaphores
    sem_init(&empty, 0, data->t_process);
    sem_init(&full, 0, 0);
    
    // Initialize the selected scheduler
    init_scheduler(scheduler);
    
    // Create arrival thread
    pthread_t arrivalThread;
    if (pthread_create(&arrivalThread, NULL, (void *)arrivalQueue, data) != 0) {
        perror("Failed to create arrival thread");
        return 1;
    }
    
    // Main simulation loop
    while (1) {
        sleep(1);  // Simulate time passing
        pthread_mutex_lock(&mutex);
        global_counter++;
        
        // Print status periodically
        if (global_counter % 5 == 0) {
            printf("\nTime: %d\n", global_counter);
            printf("Ready queue size: %d\n", readyQueue->len);
            printf("Wait queue size: %d\n", waitQueue->len);
        }
        
        pthread_mutex_unlock(&mutex);
    }
    
    // Cleanup (this will never be reached in this simple example)
    pthread_join(arrivalThread, NULL);
    sem_destroy(&empty);
    sem_destroy(&full);
    
    return 0;
}