#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "rr.h"
#include "thread_op.h"

// Global statistics
double total_waiting_time = 0;
double total_turnaround_time = 0;
int total_processes = 0;
int context_switches = 0;

// Function to execute a process for one time quantum
int execute_rr_process(burst_line *process, int current_time) {
    int time_used = 0;
    int time_remaining = TIME_QUANTUM;
    
    // If process hasn't started, calculate its waiting time
    if (process->remaining_time == process->b_time) {
        int wait_time = current_time - process->a_time;
        total_waiting_time += wait_time;
    }
    
    // Execute for the time quantum or until process completes
    while (time_remaining > 0 && process->remaining_time > 0) {
        printf("Time %d: Executing process %d (Remaining: %d)\n", 
               current_time + time_used, process->p_id, process->remaining_time - 1);
        
        // Simulate one time unit of execution
        sleep(1);
        time_remaining--;
        process->remaining_time--;
        time_used++;
        
        // Update global time counter
        pthread_mutex_lock(&mutex);
        global_counter++;
        pthread_mutex_unlock(&mutex);
    }
    
    // If process completed
    if (process->remaining_time == 0) {
        int turnaround_time = (current_time + time_used) - process->a_time;
        total_turnaround_time += turnaround_time;
        total_processes++;
        
        printf("Process %d completed. Turnaround time: %d\n",
               process->p_id, turnaround_time);
        
        // Free the process if no I/O burst
        if (process->io_burst <= 0) {
            free(process);
            return time_used;
        }
    }
    
    return time_used;
}

// Round Robin Scheduler
void *rr_scheduler(void *arg) {
    Queue *rr_queue = queue_create();  // Local queue for RR scheduling
    burst_line *current_process = NULL;
    int time_quantum_remaining = 0;
    
    while (1) {
        pthread_mutex_lock(&mutex);
        
        // Check if there are new processes in the ready queue
        while (!isEmpty(readyQueue)) {
            node *process_node = dequeue(readyQueue);
            if (process_node != NULL) {
                burst_line *process = process_node->data;
                process->remaining_time = process->b_time; // Initialize remaining time
                enqueue(rr_queue, process);
                free(process_node);
                printf("Process %d added to RR queue\n", process->p_id);
            }
        }
        
        // If current process exists and has time remaining
        if (current_process != NULL && current_process->remaining_time > 0) {
            enqueue(rr_queue, current_process);  // Put it back in the queue
            context_switches++;
        }
        
        // Get the next process from RR queue
        node *next_node = dequeue(rr_queue);
        if (next_node != NULL) {
            current_process = next_node->data;
            free(next_node);
            
            pthread_mutex_unlock(&mutex);
            
            // Execute the process for one time quantum
            int time_used = execute_rr_process(current_process, global_counter);
            
            // If process completed and has I/O burst, move to wait queue
            if (current_process->remaining_time == 0 && current_process->io_burst > 0) {
                pthread_mutex_lock(&mutex);
                add_to_waitQueue(current_process);
                pthread_mutex_unlock(&mutex);
                current_process = NULL;
            }
            
            // Sleep to simulate time passing (1 second per time unit)
            sleep(1);
            continue;
        }
        
        pthread_mutex_unlock(&mutex);
        
        // If no processes to execute, sleep briefly to prevent busy waiting
        usleep(100000); // 100ms
    }
    
    return NULL;
}

// Initialize Round Robin scheduler
void init_rr() {
    pthread_t scheduler_thread;
    if (pthread_create(&scheduler_thread, NULL, rr_scheduler, NULL) != 0) {
        perror("Failed to create RR scheduler thread");
        exit(1);
    }
}
