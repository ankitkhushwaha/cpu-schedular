#include <stdio.h>
#include <stdlib.h>
#include "../include/process.h"
#include "../include/queue.h"

// Global time quantum for Round Robin (can be set via scheduler configuration)
static int time_quantum = 4;

// Set the time quantum for Round Robin
void set_time_quantum(int quantum) {
    if (quantum > 0) {
        time_quantum = quantum;
    }
}

// Round Robin scheduling algorithm
Process *rr_schedule(Queue *ready_queue, int current_time) {
    // Simply get the next process in the ready queue (FIFO)
    Process *process = dequeue(ready_queue);
    
    // If a process is selected, set its time quantum
    if (process) {
        process->remaining_quantum = time_quantum;
    }
    
    return process;
}

// Check if the current process has exceeded its time quantum
int should_preempt_rr(Process *current_process, int current_time) {
    if (!current_process) {
        return 0;
    }
    
    // Decrement the remaining quantum
    if (current_process->remaining_quantum > 0) {
        current_process->remaining_quantum--;
    }
    
    // Return 1 if the time quantum has expired
    return (current_process->remaining_quantum <= 0);
}
