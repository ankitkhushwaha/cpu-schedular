#ifndef PROCESS_H
#define PROCESS_H

typedef enum {
    NEW,
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct {
    int is_cpu;  // 1 for CPU burst, 0 for I/O burst
    int duration;
    int remaining;
} Burst;

typedef struct {
    int pid;                // Process ID
    int arrival_time;       // When the process arrives in the system
    Burst *bursts;          // Array of CPU/IO bursts
    int burst_count;        // Number of bursts
    int current_burst;      // Index of current burst
    ProcessState state;     // Current state of the process
    int start_time;         // When the process started its current burst
    int finish_time;        // When the process completes
    int waiting_time;       // Total time spent waiting in ready queue
    int response_time;      // Time when the process first gets CPU
    int remaining_time;     // Remaining time in current CPU burst (for preemption)
    int priority;           // Priority level (for priority scheduling)
} Process;

// Function declarations
Process *create_process(int pid, int arrival_time, int *bursts, int burst_count);
void free_process(Process *process);
void print_process(Process *process);
int is_process_finished(Process *process);
int get_current_burst_remaining(Process *process);

#endif // PROCESS_H
