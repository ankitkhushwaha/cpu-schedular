#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/process.h"
#include "../include/scheduler.h"

// Maximum line length for input file
#define MAX_LINE_LENGTH 1024

// Parse a line from the input file and create a process
Process *parse_process_line(const char *line, int pid) {
    if (!line || *line == '\0' || *line == '#') {
        return NULL;  // Skip empty lines and comments
    }
    
    // Make a copy of the line for tokenization
    char line_copy[MAX_LINE_LENGTH];
    strncpy(line_copy, line, sizeof(line_copy) - 1);
    line_copy[sizeof(line_copy) - 1] = '\0';
    
    // Tokenize the line
    char *token = strtok(line_copy, " \t\n");
    if (!token) return NULL;
    
    // First token is arrival time
    int arrival_time = atoi(token);
    
    // Count the number of bursts
    int burst_count = 0;
    int max_bursts = 0;
    int *bursts = NULL;
    
    // Parse the bursts
    while ((token = strtok(NULL, " \t\n")) != NULL) {
        // Check for end of line marker
        if (strcmp(token, "-1") == 0) {
            break;
        }
        
        // Resize bursts array if needed
        if (burst_count >= max_bursts) {
            max_bursts = (max_bursts == 0) ? 10 : max_bursts * 2;
            int *new_bursts = (int *)realloc(bursts, max_bursts * sizeof(int));
            if (!new_bursts) {
                perror("Failed to allocate memory for bursts");
                free(bursts);
                return NULL;
            }
            bursts = new_bursts;
        }
        
        // Parse burst duration
        int duration = atoi(token);
        if (duration < 0) {
            fprintf(stderr, "Invalid burst duration: %d\n", duration);
            free(bursts);
            return NULL;
        }
        
        bursts[burst_count++] = duration;
    }
    
    // Create the process
    Process *process = create_process(pid, arrival_time, bursts, burst_count);
    free(bursts);
    
    return process;
}

// Read processes from a file
Process **read_process_file(const char *filename, int *count) {
    if (!filename || !count) return NULL;
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return NULL;
    }
    
    // Initial allocation for processes
    int capacity = 10;
    Process **processes = (Process **)malloc(capacity * sizeof(Process *));
    if (!processes) {
        perror("Failed to allocate memory for processes");
        fclose(file);
        return NULL;
    }
    
    char line[MAX_LINE_LENGTH];
    *count = 0;
    int pid = 1;  // Start PIDs from 1
    
    // Read file line by line
    while (fgets(line, sizeof(line), file)) {
        // Skip empty lines and comments
        if (line[0] == '\n' || line[0] == '#' || line[0] == '\0') {
            continue;
        }
        
        // Remove trailing newline
        size_t len = strlen(line);
        if (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r')) {
            line[--len] = '\0';
        }
        if (len > 0 && (line[len-1] == '\r')) {
            line[--len] = '\0';
        }
        
        // Parse the process line
        Process *process = parse_process_line(line, pid++);
        if (process) {
            // Resize array if needed
            if (*count >= capacity) {
                capacity *= 2;
                Process **new_processes = (Process **)realloc(processes, capacity * sizeof(Process *));
                if (!new_processes) {
                    perror("Failed to reallocate memory for processes");
                    // Free all previously allocated processes
                    for (int i = 0; i < *count; i++) {
                        free_process(processes[i]);
                    }
                    free(processes);
                    fclose(file);
                    return NULL;
                }
                processes = new_processes;
            }
            
            processes[(*count)++] = process;
        }
    }
    
    fclose(file);
    return processes;
}

// Free an array of processes
void free_processes(Process **processes, int count) {
    if (!processes) return;
    
    for (int i = 0; i < count; i++) {
        free_process(processes[i]);
    }
    free(processes);
}
