#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/process.h"

// Create a new process with the given bursts
Process *create_process(int pid, int arrival_time, int *bursts, int burst_count) {
    Process *process = (Process *)malloc(sizeof(Process));
    if (!process) {
        perror("Failed to allocate memory for process");
        return NULL;
    }
    
    process->pid = pid;
    process->arrival_time = arrival_time;
    process->burst_count = burst_count / 2 + (burst_count % 2); // Each CPU burst is followed by an I/O burst
    process->current_burst = 0;
    process->state = NEW;
    process->start_time = -1;
    process->finish_time = -1;
    process->waiting_time = 0;
    process->response_time = -1; // -1 means not responded yet
    process->remaining_time = 0;
    process->priority = 0; // Default priority
    
    // Allocate memory for bursts
    process->bursts = (Burst *)malloc(process->burst_count * sizeof(Burst));
    if (!process->bursts) {
        perror("Failed to allocate memory for bursts");
        free(process);
        return NULL;
    }
    
    // Initialize bursts
    for (int i = 0; i < burst_count; i++) {
        if (i % 2 == 0) {
            // CPU burst
            process->bursts[i/2].is_cpu = 1;
            process->bursts[i/2].duration = bursts[i];
            process->bursts[i/2].remaining = bursts[i];
        } else {
            // I/O burst
            process->bursts[i/2].is_cpu = 0;
            process->bursts[i/2].duration = bursts[i];
            process->bursts[i/2].remaining = bursts[i];
        }
    }
    
    // If the last burst is a CPU burst, add a zero I/O burst
    if (burst_count % 2 == 1) {
        process->bursts[process->burst_count-1].is_cpu = 0;
        process->bursts[process->burst_count-1].duration = 0;
        process->bursts[process->burst_count-1].remaining = 0;
    }
    
    return process;
}

// Free memory used by a process
void free_process(Process *process) {
    if (process) {
        if (process->bursts) {
            free(process->bursts);
        }
        free(process);
    }
}

// Print process information
void print_process(Process *process) {
    if (!process) return;
    
    printf("Process %d (Arrival: %d, State: %d, Current Burst: %d)\n", 
           process->pid, process->arrival_time, process->state, process->current_burst);
    printf("Bursts: ");
    for (int i = 0; i < process->burst_count; i++) {
        printf("%s%d ", process->bursts[i].is_cpu ? "CPU:" : "I/O:", 
               process->bursts[i].duration);
    }
    printf("\n");
}

// Check if process has finished all bursts
int is_process_finished(Process *process) {
    if (!process) return 1;
    return (process->current_burst >= process->burst_count);
}

// Get remaining time in current CPU burst
int get_current_burst_remaining(Process *process) {
    if (!process || is_process_finished(process)) {
        return 0;
    }
    return process->bursts[process->current_burst].remaining;
}
