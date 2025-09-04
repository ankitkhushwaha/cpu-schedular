#include <stdio.h>
#include <stdlib.h>
#include "../include/process.h"
#include "../include/queue.h"

// Helper function to find the process with the shortest next CPU burst
static Process *find_shortest_job(Queue *ready_queue) {
    if (is_empty(ready_queue)) {
        return NULL;
    }
    
    Node *current = ready_queue->head;
    Process *shortest_job = current->process;
    int shortest_burst = shortest_job->bursts[shortest_job->current_burst].duration;
    
    // Find the process with the shortest next CPU burst
    while (current != NULL) {
        Process *p = current->process;
        int burst = p->bursts[p->current_burst].duration;
        
        if (burst < shortest_burst) {
            shortest_burst = burst;
            shortest_job = p;
        }
        
        current = current->next;
    }
    
    // Remove the selected process from the ready queue
    return dequeue_process(ready_queue, shortest_job);
}

// SJF (non-preemptive) scheduling algorithm
Process *sjf_schedule(Queue *ready_queue, int current_time) {
    // Find and return the process with the shortest next CPU burst
    return find_shortest_job(ready_queue);
}
