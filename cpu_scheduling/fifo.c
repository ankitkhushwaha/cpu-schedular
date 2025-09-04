#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "fifo.h"
#include "thread_op.h"

// Global variables
extern Queue *readyQueue;
extern Queue *waitQueue;
extern pthread_mutex_t mutex;

double total_waiting_time = 0;
double total_turnaround_time = 0;
int total_processes = 0;

// Function to execute a process
void execute_process(burst_line *process) {
    printf("Executing process %d (Burst: %d, Priority: %d)\n", 
           process->p_id, process->b_time, process->priority);
    
    // Simulate CPU burst time
    sleep(process->b_time);
    
    // Calculate waiting time and turnaround time
    int waiting_time = global_counter - process->a_time;
    int turnaround_time = waiting_time + process->b_time;
    
    total_waiting_time += waiting_time;
    total_turnaround_time += turnaround_time;
    total_processes++;
    
    printf("Process %d completed. Waiting time: %d, Turnaround time: %d\n",
           process->p_id, waiting_time, turnaround_time);
}

// FCFS Scheduler
void *fcfs_scheduler(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        
        if (!isEmpty(readyQueue)) {
            // Get the first process from ready queue
            node *process_node = dequeue(readyQueue);
            if (process_node != NULL) {
                burst_line *process = process_node->data;
                free(process_node); // Free the node after getting the data
                
                pthread_mutex_unlock(&mutex);
                
                // Execute the process
                execute_process(process);
                
                // If process has I/O burst, add to wait queue
                if (process->io_burst > 0) {
                    pthread_mutex_lock(&mutex);
                    add_to_waitQueue(process);
                    pthread_mutex_unlock(&mutex);
                } else {
                    // Free the process if no I/O burst
                    free(process);
                }
                
                continue; // Go back to check the ready queue
            }
        }
        
        pthread_mutex_unlock(&mutex);
        
        // Sleep briefly to prevent busy waiting
        usleep(1000); // 1ms
    }
    
    return NULL;
}

// Initialize FCFS scheduler
void init_fcfs() {
    pthread_t scheduler_thread;
    pthread_create(&scheduler_thread, NULL, fcfs_scheduler, NULL);
}
