#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "../include/process.h"
#include "../include/queue.h"

// Helper function to find the process with the highest priority (lowest priority number)
static Process *find_highest_priority(Queue *ready_queue, Process *current_process, int preemptive) {
    if (is_empty(ready_queue) && !current_process) {
        return NULL;
    }
    
    Process *highest_priority = current_process;
    int highest_prio = (current_process != NULL) ? current_process->priority : INT_MAX;
    
    // Check all processes in the ready queue
    Node *current = ready_queue->head;
    while (current != NULL) {
        Process *p = current->process;
        
        // Lower number means higher priority
        if (p->priority < highest_prio) {
            highest_prio = p->priority;
            highest_priority = p;
        }
        
        current = current->next;
    }
    
    // If preemptive is disabled, only choose a new process if none is running
    if (!preemptive && current_process != NULL) {
        return NULL;
    }
    
    // If the current process is still the highest priority, return NULL to continue with it
    if (highest_priority == current_process) {
        return NULL;
    }
    
    // Otherwise, return the new highest priority process (removed from the ready queue)
    return dequeue_process(ready_queue, highest_priority);
}

// Priority scheduling algorithm (preemptive or non-preemptive)
Process *priority_schedule(Queue *ready_queue, int current_time) {
    // The current process is passed as the first parameter (NULL if none running)
    // The third parameter (1) enables preemptive behavior
    return find_highest_priority(ready_queue, NULL, 1);
}
