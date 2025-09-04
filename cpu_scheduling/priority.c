#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "priority.h"
#include "thread_op.h"

// Global statistics
double total_waiting_time = 0;
double total_turnaround_time = 0;
double total_response_time = 0;
int total_processes = 0;
int context_switches = 0;

// Comparison function for qsort to sort by priority (lower number = higher priority)
int compare_priority(const void *a, const void *b) {
    const burst_line *pa = *(const burst_line **)a;
    const burst_line *pb = *(const burst_line **)b;
    
    // First sort by priority, then by arrival time for same priority
    if (pa->priority != pb->priority) {
        return pa->priority - pb->priority;
    }
    return pa->a_time - pb->a_time;
}

// Function to execute a process
void execute_priority_process(burst_line *process, int current_time, int *time_used) {
    // Calculate response time (first time process gets CPU)
    if (process->response_time == -1) {
        process->response_time = current_time - process->a_time;
        total_response_time += process->response_time;
    }
    
    // For preemptive scheduling, we execute for 1 time unit at a time
    #if PREEMPTIVE
    int execution_time = 1;
    #else
    // For non-preemptive, execute the entire burst
    int execution_time = process->remaining_time;
    #endif
    
    printf("Time %d: Executing process %d (Priority: %d, Remaining: %d)\n", 
           current_time, process->p_id, process->priority, process->remaining_time - execution_time);
    
    // Simulate CPU execution
    sleep(execution_time);
    
    // Update process state
    process->remaining_time -= execution_time;
    process->last_executed = current_time + execution_time;
    *time_used = execution_time;
    
    // Update global time counter
    pthread_mutex_lock(&mutex);
    global_counter += execution_time;
    pthread_mutex_unlock(&mutex);
    
    // If process completed
    if (process->remaining_time == 0) {
        int completion_time = current_time + execution_time;
        int turnaround_time = completion_time - process->a_time;
        int waiting_time = turnaround_time - (process->b_time - process->remaining_time);
        
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
            return;
        } else {
            // Free the process if no I/O burst
            free(process);
            return;
        }
    }
}

// Priority Scheduler
void *priority_scheduler(void *arg) {
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
                    processes[i] = process_node->data;
                    // Initialize response time if not set
                    if (processes[i]->response_time == -1) {
                        processes[i]->response_time = -1;
                    }
                    free(process_node);
                    i++;
                }
            }
            
            // Sort the processes by priority
            qsort(processes, count, sizeof(burst_line *), compare_priority);
            
            // Put the sorted processes back in the queue
            for (i = 0; i < count; i++) {
                enqueue(readyQueue, processes[i]);
            }
            
            free(processes);
            
            // Get the highest priority process
            node *process_node = dequeue(readyQueue);
            if (process_node != NULL) {
                burst_line *process = process_node->data;
                free(process_node);
                
                // Unlock before executing the process
                pthread_mutex_unlock(&mutex);
                
                // Execute the process
                int time_used = 0;
                execute_priority_process(process, global_counter, &time_used);
                
                // For preemptive scheduling, put the process back in the queue if not completed
                #if PREEMPTIVE
                if (process->remaining_time > 0) {
                    pthread_mutex_lock(&mutex);
                    enqueue(readyQueue, process);
                    context_switches++;
                    pthread_mutex_unlock(&mutex);
                }
                #endif
                
                continue; // Go back to check the ready queue
            }
        }
        
        pthread_mutex_unlock(&mutex);
        
        // If no processes to execute, sleep briefly to prevent busy waiting
        usleep(100000); // 100ms
    }
    
    return NULL;
}

// Initialize Priority scheduler
void init_priority() {
    pthread_t scheduler_thread;
    if (pthread_create(&scheduler_thread, NULL, priority_scheduler, NULL) != 0) {
        perror("Failed to create Priority scheduler thread");
        exit(1);
    }
}
