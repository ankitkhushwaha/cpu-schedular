#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "sjf.h"
#include "thread_op.h"

// Global statistics
double total_waiting_time = 0;
double total_turnaround_time = 0;
double total_response_time = 0;
int total_processes = 0;

// Comparison function for qsort to sort by burst time (SJF)
int compare_burst_time(const void *a, const void *b) {
    const burst_line *pa = *(const burst_line **)a;
    const burst_line *pb = *(const burst_line **)b;
    
    // First sort by remaining time, then by arrival time
    if (pa->b_time != pb->b_time) {
        return pa->b_time - pb->b_time;
    }
    return pa->a_time - pb->a_time;
}

// Function to execute a process
void execute_sjf_process(burst_line *process, int current_time) {
    // Calculate response time (first time process gets CPU)
    if (process->response_time == -1) {
        process->response_time = current_time - process->a_time;
        total_response_time += process->response_time;
    }
    
    printf("Time %d: Executing process %d (Burst: %d)\n", 
           current_time, process->p_id, process->b_time);
    
    // Simulate CPU burst time
    sleep(process->b_time);
    
    // Update global time counter
    pthread_mutex_lock(&mutex);
    global_counter += process->b_time;
    pthread_mutex_unlock(&mutex);
    
    // Calculate waiting time and turnaround time
    int completion_time = current_time + process->b_time;
    int turnaround_time = completion_time - process->a_time;
    int waiting_time = turnaround_time - process->b_time;
    
    // Update statistics
    total_waiting_time += waiting_time;
    total_turnaround_time += turnaround_time;
    total_processes++;
    
    printf("Process %d completed. Waiting time: %d, Turnaround time: %d\n",
           process->p_id, waiting_time, turnaround_time);
    
    // If process has I/O burst, add to wait queue
    if (process->io_burst > 0) {
        pthread_mutex_lock(&mutex);
        add_to_waitQueue(process);
        pthread_mutex_unlock(&mutex);
    } else {
        // Free the process if no I/O burst
        free(process);
    }
}

// SJF Scheduler
void *sjf_scheduler(void *arg) {
    while (1) {
        pthread_mutex_lock(&mutex);
        
        // Check if there are processes in the ready queue
        if (!isEmpty(readyQueue)) {
            // Count the number of processes in the ready queue
            int count = 0;
            node *current = readyQueue->front;
            while (current != NULL) {
                count++;
                current = current->next;
            }
            
            // Create an array to sort the processes
            burst_line **processes = (burst_line **)malloc(count * sizeof(burst_line *));
            if (processes == NULL) {
                perror("Memory allocation failed");
                exit(1);
            }
            
            // Extract processes from the queue
            int i = 0;
            while (!isEmpty(readyQueue)) {
                node *process_node = dequeue(readyQueue);
                if (process_node != NULL) {
                    processes[i++] = process_node->data;
                    free(process_node);
                }
            }
            
            // Sort the processes by burst time (SJF)
            qsort(processes, count, sizeof(burst_line *), compare_burst_time);
            
            // Put the sorted processes back in the queue
            for (i = 0; i < count; i++) {
                enqueue(readyQueue, processes[i]);
            }
            
            free(processes);
            
            // Get the process with the shortest burst time
            node *process_node = dequeue(readyQueue);
            if (process_node != NULL) {
                burst_line *process = process_node->data;
                free(process_node);
                
                // Unlock before executing the process
                pthread_mutex_unlock(&mutex);
                
                // Execute the process
                execute_sjf_process(process, global_counter);
                
                continue; // Go back to check the ready queue
            }
        }
        
        pthread_mutex_unlock(&mutex);
        
        // If no processes to execute, sleep briefly to prevent busy waiting
        usleep(100000); // 100ms
    }
    
    return NULL;
}

// Initialize SJF scheduler
void init_sjf() {
    pthread_t scheduler_thread;
    if (pthread_create(&scheduler_thread, NULL, sjf_scheduler, NULL) != 0) {
        perror("Failed to create SJF scheduler thread");
        exit(1);
    }
}
