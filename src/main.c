#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/process.h"
#include "../include/scheduler.h"
#include "../include/queue.h"

// Function to print usage information
void print_usage(const char *program_name) {
    printf("Usage: %s <scheduler> <input_file> [time_quantum]\n", program_name);
    printf("Schedulers:\n");
    printf("  fcfs     First-Come-First-Serve\n");
    printf("  sjf      Shortest Job First (non-preemptive)\n");
    printf("  srtf     Shortest Remaining Time First (preemptive SJF)\n");
    printf("  priority Priority Scheduling (preemptive)\n");
    printf("  rr       Round Robin (requires time_quantum)\n");
    printf("\nExample: %s fcfs test_cases/process1.dat\n", program_name);
    printf("Example: %s rr test_cases/process2.dat 4\n", program_name);
}

// Function to parse scheduler type from string
SchedulerType parse_scheduler_type(const char *type_str) {
    if (strcmp(type_str, "fcfs") == 0) return FCFS;
    if (strcmp(type_str, "sjf") == 0) return SJF;
    if (strcmp(type_str, "srtf") == 0) return SRTF;
    if (strcmp(type_str, "priority") == 0) return PRIORITY;
    if (strcmp(type_str, "rr") == 0) return RR;
    return -1; // Invalid type
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc < 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    // Parse scheduler type
    SchedulerType scheduler_type = parse_scheduler_type(argv[1]);
    if (scheduler_type == -1) {
        fprintf(stderr, "Error: Invalid scheduler type '%s'\n", argv[1]);
        print_usage(argv[0]);
        return 1;
    }
    
    // Parse time quantum for Round Robin
    int time_quantum = 4; // Default value
    if (scheduler_type == RR) {
        if (argc < 4) {
            fprintf(stderr, "Error: Round Robin scheduler requires a time quantum\n");
            print_usage(argv[0]);
            return 1;
        }
        time_quantum = atoi(argv[3]);
        if (time_quantum <= 0) {
            fprintf(stderr, "Error: Time quantum must be a positive integer\n");
            return 1;
        }
    }
    
    // Read input file
    int process_count = 0;
    Process **processes = read_process_file(argv[2], &process_count);
    if (!processes || process_count == 0) {
        fprintf(stderr, "Error: No valid processes found in file '%s'\n", argv[2]);
        return 1;
    }
    
    printf("Loaded %d processes from '%s'\n", process_count, argv[2]);
    
    // Initialize scheduler configuration
    SchedulerConfig config;
    config.type = scheduler_type;
    config.time_quantum = time_quantum;
    config.num_processors = 1; // Single processor for Part I
    
    // Create and initialize scheduler
    Scheduler *scheduler = create_scheduler(config);
    if (!scheduler) {
        fprintf(stderr, "Error: Failed to create scheduler\n");
        free_processes(processes, process_count);
        return 1;
    }
    
    // Add processes to the scheduler
    for (int i = 0; i < process_count; i++) {
        add_process(scheduler, processes[i]);
    }
    
    // Run the scheduler
    printf("\n=== Starting Simulation ===\n");
    run_scheduler(scheduler);
    
    // Print statistics
    print_stats(scheduler);
    
    // Cleanup
    free_scheduler(scheduler);
    free_processes(processes, process_count);
    
    return 0;
}
