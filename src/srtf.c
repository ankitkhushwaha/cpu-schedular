#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/process.h"
#include "../include/queue.h"

// Helper function to find the process with the shortest remaining time
static Process *find_shortest_remaining_time(Queue *ready_queue, Process *current_process) {
    if (is_empty(ready_queue) && !current_process) {
        return NULL;
    }
    
    Process *shortest_job = current_process;
    int shortest_remaining = (current_process != NULL) ? 
        current_process->bursts[current_process->current_burst].remaining : INT_MAX;
    
    // Check all processes in the ready queue
    Node *current = ready_queue->head;
    while (current != NULL) {
        Process *p = current->process;
        int remaining = p->bursts[p->current_burst].remaining;
        
        if (remaining < shortest_remaining) {
            shortest_remaining = remaining;
            shortest_job = p;
        }
        
        current = current->next;
    }
    
    // If the current process is still the shortest, return NULL to continue with it
    if (shortest_job == current_process) {
        return NULL;
    }
    
    // Otherwise, return the new shortest job (removed from the ready queue)
    return dequeue_process(ready_queue, shortest_job);
}

// SRTF (preemptive SJF) scheduling algorithm
Process *srtf_schedule(Queue *ready_queue, int current_time) {
    // The current process is passed as the first parameter (NULL if none running)
    // We need to check if we should preempt it with a shorter job
    return find_shortest_remaining_time(ready_queue, NULL);
}
