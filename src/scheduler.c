#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/scheduler.h"
#include "../include/process.h"
#include "../include/queue.h"

// Create a new scheduler with the given configuration
Scheduler *create_scheduler(SchedulerConfig config) {
    Scheduler *scheduler = (Scheduler *)calloc(1, sizeof(Scheduler));
    if (!scheduler) {
        perror("Failed to allocate memory for scheduler");
        return NULL;
    }
    
    scheduler->config = config;
    scheduler->ready_queue = create_queue();
    scheduler->wait_queue = create_queue();
    scheduler->current_time = 0;
    scheduler->num_processes = 0;
    scheduler->num_running = 0;
    
    // Set the scheduling algorithm based on the scheduler type
    switch (config.type) {
        case FCFS:
            scheduler->schedule = fcfs_schedule;
            break;
        case SJF:
            scheduler->schedule = sjf_schedule;
            break;
        case SRTF:
            scheduler->schedule = srtf_schedule;
            break;
        case PRIORITY:
            scheduler->schedule = priority_schedule;
            break;
        case RR:
            scheduler->schedule = rr_schedule;
            break;
        default:
            fprintf(stderr, "Unknown scheduler type\n");
            free(scheduler);
            return NULL;
    }
    
    // Allocate memory for running processes (for multi-processor support)
    scheduler->running_processes = (Process **)calloc(config.num_processors, sizeof(Process *));
    if (!scheduler->running_processes) {
        perror("Failed to allocate memory for running processes");
        free(scheduler);
        return NULL;
    }
    
    // Initialize statistics
    memset(&scheduler->stats, 0, sizeof(SchedulerStats));
    
    return scheduler;
}

// Free all resources used by the scheduler
void free_scheduler(Scheduler *scheduler) {
    if (!scheduler) return;
    
    // Free queues (but not the processes, as they're owned by the caller)
    if (scheduler->ready_queue) free_queue(scheduler->ready_queue);
    if (scheduler->wait_queue) free_queue(scheduler->wait_queue);
    
    // Free running processes array
    if (scheduler->running_processes) free(scheduler->running_processes);
    
    // Free the scheduler itself
    free(scheduler);
}

// Add a process to the scheduler
void add_process(Scheduler *scheduler, Process *process) {
    if (!scheduler || !process) return;
    
    // Resize the processes array
    scheduler->processes = (Process **)realloc(scheduler->processes, 
                                             (scheduler->num_processes + 1) * sizeof(Process *));
    if (!scheduler->processes) {
        perror("Failed to allocate memory for processes");
        return;
    }
    
    // Add the process to the array and update the count
    scheduler->processes[scheduler->num_processes++] = process;
    
    // Add the process to the ready queue if it arrives at time 0
    if (process->arrival_time == 0) {
        process->state = READY;
        enqueue(scheduler->ready_queue, process);
    }
    
    // Update statistics
    scheduler->stats.total_processes++;
}

// Update waiting times for all processes in the ready queue
void update_waiting_times(Queue *queue, int time_elapsed) {
    if (!queue || time_elapsed <= 0) return;
    
    Node *current = queue->head;
    while (current) {
        if (current->process->state == READY) {
            current->process->waiting_time += time_elapsed;
        }
        current = current->next;
    }
}

// Check if all processes have finished
int all_processes_finished(Process **processes, int count) {
    for (int i = 0; i < count; i++) {
        if (!is_process_finished(processes[i])) {
            return 0;
        }
    }
    return 1;
}

// Print schedule header
void print_schedule_header() {
    printf("Time\tCPU\tProcess\tEvent\n");
    printf("----\t---\t-------\t-----\n");
}

// Print a schedule event
void print_schedule_event(int time, int cpu_id, int pid, int burst_num, const char *event) {
    printf("%4d\t%3d\t%7d\t%s", time, cpu_id, pid, event);
    if (burst_num >= 0) {
        printf(" (Burst %d)", burst_num + 1);
    }
    printf("\n");
}

// First-Come-First-Serve scheduling algorithm
Process *fcfs_schedule(Queue *ready_queue, int current_time) {
    return dequeue(ready_queue);
}

// Run the scheduler
void run_scheduler(Scheduler *scheduler) {
    if (!scheduler) return;
    
    clock_t start_time = clock();
    int time_elapsed = 0;
    int cpu_idle = 1;
    
    print_schedule_header();
    
    // Main simulation loop
    while (!all_processes_finished(scheduler->processes, scheduler->num_processes)) {
        // Check for new arrivals
        for (int i = 0; i < scheduler->num_processes; i++) {
            Process *p = scheduler->processes[i];
            if (p->arrival_time == scheduler->current_time && p->state == NEW) {
                p->state = READY;
                enqueue(scheduler->ready_queue, p);
                print_schedule_event(scheduler->current_time, 0, p->pid, -1, "Arrived");
            }
        }
        
        // Check for completed I/O operations
        Node *current = scheduler->wait_queue->head;
        while (current) {
            Process *p = current->process;
            if (p->state == WAITING) {
                Burst *current_burst = &p->bursts[p->current_burst];
                if (--current_burst->remaining <= 0) {
                    // I/O burst completed, move to next burst
                    p->current_burst++;
                    if (p->current_burst < p->burst_count) {
                        // More bursts to process
                        p->state = READY;
                        enqueue(scheduler->ready_queue, p);
                        print_schedule_event(scheduler->current_time, 0, p->pid, p->current_burst, "I/O Complete");
                    }
                }
            }
            current = current->next;
        }
        
        // Schedule processes on available CPUs
        for (int cpu = 0; cpu < scheduler->config.num_processors; cpu++) {
            if (!scheduler->running_processes[cpu] && !is_empty(scheduler->ready_queue)) {
                // Get the next process to run
                Process *p = scheduler->schedule(scheduler->ready_queue, scheduler->current_time);
                if (p) {
                    p->state = RUNNING;
                    scheduler->running_processes[cpu] = p;
                    scheduler->num_running++;
                    
                    // Record response time (first time process gets CPU)
                    if (p->response_time == -1) {
                        p->response_time = scheduler->current_time - p->arrival_time;
                        scheduler->stats.avg_response_time += p->response_time;
                    }
                    
                    print_schedule_event(scheduler->current_time, cpu, p->pid, p->current_burst, "Started");
                }
            }
        }
        
        // Execute one time unit on each CPU
        for (int cpu = 0; cpu < scheduler->config.num_processors; cpu++) {
            Process *p = scheduler->running_processes[cpu];
            if (p) {
                Burst *current_burst = &p->bursts[p->current_burst];
                current_burst->remaining--;
                
                // Check if the current CPU burst is completed
                if (current_burst->remaining <= 0) {
                    // Move to the next burst (I/O burst or completion)
                    p->current_burst++;
                    
                    if (p->current_burst < p->burst_count) {
                        // More bursts to process
                        if (p->current_burst % 2 == 1) {
                            // Next is an I/O burst
                            p->state = WAITING;
                            enqueue(scheduler->wait_queue, p);
                            print_schedule_event(scheduler->current_time + 1, cpu, p->pid, p->current_burst, "I/O Start");
                        } else {
                            // Next is a CPU burst, put back in ready queue
                            p->state = READY;
                            enqueue(scheduler->ready_queue, p);
                        }
                    } else {
                        // Process completed
                        p->state = TERMINATED;
                        p->finish_time = scheduler->current_time + 1;
                        int turnaround = p->finish_time - p->arrival_time;
                        scheduler->stats.avg_turnaround_time += turnaround;
                        if (turnaround > scheduler->stats.max_turnaround_time) {
                            scheduler->stats.max_turnaround_time = turnaround;
                        }
                        scheduler->stats.completed_processes++;
                        print_schedule_event(scheduler->current_time + 1, cpu, p->pid, -1, "Completed");
                    }
                    
                    scheduler->running_processes[cpu] = NULL;
                    scheduler->num_running--;
                }
            }
        }
        
        // Update waiting times for processes in the ready queue
        update_waiting_times(scheduler->ready_queue, 1);
        
        // Increment simulation time
        scheduler->current_time++;
        time_elapsed++;
    }
    
    // Calculate final statistics
    clock_t end_time = clock();
    double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    // Calculate average statistics
    if (scheduler->stats.completed_processes > 0) {
        scheduler->stats.avg_turnaround_time /= scheduler->stats.completed_processes;
        scheduler->stats.avg_waiting_time = 0;
        scheduler->stats.avg_response_time /= scheduler->stats.completed_processes;
        
        // Calculate total waiting time
        for (int i = 0; i < scheduler->num_processes; i++) {
            scheduler->stats.avg_waiting_time += scheduler->processes[i]->waiting_time;
        }
        scheduler->stats.avg_waiting_time /= scheduler->stats.completed_processes;
        
        // Calculate CPU utilization
        scheduler->stats.cpu_utilization = 100.0 * (scheduler->current_time - time_elapsed) / scheduler->current_time;
        
        // Calculate throughput
        scheduler->stats.throughput = (double)scheduler->stats.completed_processes / scheduler->current_time;
    }
}

// Print scheduler statistics
void print_stats(Scheduler *scheduler) {
    if (!scheduler) return;
    
    printf("\n=== Scheduler Statistics ===\n");
    printf("Scheduler Type: ");
    switch (scheduler->config.type) {
        case FCFS: printf("First-Come-First-Serve (FCFS)\n"); break;
        case SJF: printf("Shortest Job First (SJF)\n"); break;
        case SRTF: printf("Shortest Remaining Time First (SRTF)\n"); break;
        case PRIORITY: printf("Priority Scheduling\n"); break;
        case RR: printf("Round Robin (Time Quantum: %d)\n", scheduler->config.time_quantum); break;
        default: printf("Unknown\n");
    }
    
    printf("Number of Processors: %d\n", scheduler->config.num_processors);
    printf("Total Processes: %d\n", scheduler->stats.total_processes);
    printf("Completed Processes: %d\n", scheduler->stats.completed_processes);
    printf("Average Turnaround Time: %.2f\n", scheduler->stats.avg_turnaround_time);
    printf("Maximum Turnaround Time: %d\n", scheduler->stats.max_turnaround_time);
    printf("Average Waiting Time: %.2f\n", scheduler->stats.avg_waiting_time);
    printf("Average Response Time: %.2f\n", scheduler->stats.avg_response_time);
    printf("Total Context Switches: %d\n", scheduler->stats.total_context_switches);
    printf("CPU Utilization: %.2f%%\n", scheduler->stats.cpu_utilization);
    printf("Throughput: %.4f processes per time unit\n", scheduler->stats.throughput);
}

// Stub implementations for other scheduling algorithms
Process *sjf_schedule(Queue *ready_queue, int current_time) {
    // Will be implemented in the next step
    return NULL;
}

Process *srtf_schedule(Queue *ready_queue, int current_time) {
    // Will be implemented in the next step
    return NULL;
}

Process *priority_schedule(Queue *ready_queue, int current_time) {
    // Will be implemented in the next step
    return NULL;
}

Process *rr_schedule(Queue *ready_queue, int current_time) {
    // Will be implemented in the next step
    return NULL;
}
