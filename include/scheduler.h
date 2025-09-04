#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdbool.h>
#include "process.h"
#include "queue.h"

// Scheduler statistics
typedef struct {
    int total_processes;         // Total number of processes
    int completed_processes;     // Number of completed processes
    double avg_turnaround_time;  // Average turnaround time
    int max_turnaround_time;     // Maximum turnaround time
    double avg_waiting_time;     // Average waiting time
    double avg_response_time;    // Average response time
    int total_context_switches;  // Total number of context switches
    double cpu_utilization;      // CPU utilization percentage
    double throughput;           // Processes completed per time unit
} SchedulerStats;

// Scheduler type
typedef enum {
    FCFS,      // First-Come-First-Serve
    SJF,       // Shortest Job First (non-preemptive)
    SRTF,      // Shortest Remaining Time First (preemptive SJF)
    PRIORITY,  // Priority scheduling (preemptive)
    RR         // Round Robin
} SchedulerType;

// Scheduler configuration
typedef struct {
    SchedulerType type;   // Type of scheduler
    int time_quantum;     // Time quantum for RR (0 for others)
    int num_processors;   // Number of processors (for Part II)
    bool preemptive;      // Whether the scheduler is preemptive
} SchedulerConfig;

// Function pointer type for scheduling algorithm
typedef Process* (*SchedulingAlgorithm)(Queue *ready_queue, int current_time);
typedef int (*ShouldPreemptFunc)(Process *current_process, int current_time);

// Scheduler structure
typedef struct Scheduler {
    SchedulerConfig config;       // Scheduler configuration
    SchedulerStats stats;         // Scheduler statistics
    Queue *ready_queue;           // Ready queue
    Queue *wait_queue;            // I/O wait queue
    Process **processes;          // Array of all processes
    int num_processes;            // Number of processes
    int current_time;             // Current simulation time
    Process **running_processes;  // Currently running processes (for multi-processor)
    int num_running;              // Number of currently running processes
    SchedulingAlgorithm schedule; // Scheduling algorithm function
    ShouldPreemptFunc should_preempt; // Function to check if preemption is needed
    
    // For RR scheduling
    int time_quantum;             // Current time quantum (for RR)
    int remaining_quantum;        // Remaining time in current quantum (for RR)
} Scheduler;

// Function declarations
Scheduler *create_scheduler(SchedulerConfig config);
void free_scheduler(Scheduler *scheduler);
void add_process(Scheduler *scheduler, Process *process);
void run_scheduler(Scheduler *scheduler);
void print_stats(Scheduler *scheduler);

// Scheduling algorithms
Process *fcfs_schedule(Queue *ready_queue, int current_time);
Process *sjf_schedule(Queue *ready_queue, int current_time);
Process *srtf_schedule(Queue *ready_queue, int current_time);
Process *priority_schedule(Queue *ready_queue, int current_time);
Process *rr_schedule(Queue *ready_queue, int current_time);
int should_preempt_rr(Process *current_process, int current_time);

// Helper functions
void update_waiting_times(Queue *queue, int time_elapsed);
int all_processes_finished(Process **processes, int count);
void print_schedule_header(void);
void print_schedule_event(int time, int cpu_id, int pid, int burst_num, const char *event);

// File I/O functions
Process **read_process_file(const char *filename, int *count);
void free_processes(Process **processes, int count);

// Queue operations
Process *dequeue_process(Queue *queue, Process *process);

// Process management
int is_process_finished(Process *process);

// Time management
void update_current_time(Scheduler *scheduler, int time_elapsed);

// Statistics
void update_statistics(Scheduler *scheduler, Process *process);

// Debugging
void print_process_state(Process *process);
void print_queue_state(Queue *queue, const char *queue_name);

#endif /* SCHEDULER_H */
Process *fcfs_schedule(Queue *ready_queue, int current_time);
Process *sjf_schedule(Queue *ready_queue, int current_time);
Process *srtf_schedule(Queue *ready_queue, int current_time);
Process *priority_schedule(Queue *ready_queue, int current_time);
Process *rr_schedule(Queue *ready_queue, int current_time);

// Helper functions
void update_waiting_times(Queue *queue, int time_elapsed);
int all_processes_finished(Process **processes, int count);
void print_schedule_header();
void print_schedule_event(int time, int cpu_id, int pid, int burst_num, const char *event);

#endif // SCHEDULER_H
