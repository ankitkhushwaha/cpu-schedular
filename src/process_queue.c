#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "../include/queue.h"
#include "../include/process.h"

// Helper function to create a new node
static Node *create_node(Process *process) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {
        perror("Failed to allocate memory for node");
        return NULL;
    }
    node->process = process;
    node->next = NULL;
    return node;
}

// Create a new empty queue
Queue *create_queue() {
    Queue *queue = (Queue *)calloc(1, sizeof(Queue));
    if (!queue) {
        perror("Failed to allocate memory for queue");
        return NULL;
    }
    queue->head = queue->tail = NULL;
    queue->size = 0;
    return queue;
}

// Add a process to the end of the queue
void enqueue(Queue *queue, Process *process) {
    if (!queue || !process) return;
    
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        perror("Failed to allocate memory for queue node");
        return;
    }
    
    new_node->process = process;
    new_node->next = NULL;
    
    if (queue->tail) {
        queue->tail->next = new_node;
    } else {
        queue->head = new_node;
    }
    queue->tail = new_node;
    queue->size++;
}

// Remove and return the process at the front of the queue
Process *dequeue(Queue *queue) {
    if (!queue || !queue->head) return NULL;
    
    Node *temp = queue->head;
    Process *process = temp->process;
    queue->head = queue->head->next;
    
    if (!queue->head) {
        queue->tail = NULL;
    }
    
    free(temp);
    queue->size--;
    return process;
}

// Peek at the process at the front of the queue without removing it
Process *peek(Queue *queue) {
    if (!queue || !queue->head) return NULL;
    return queue->head->process;
}

// Check if the queue is empty
int is_empty(Queue *queue) {
    return (queue == NULL || queue->head == NULL);
}

// Get the number of processes in the queue
int queue_size(Queue *queue) {
    return (queue) ? queue->size : 0;
}

// Free all resources used by the queue
void free_queue(Queue *queue) {
    if (!queue) return;
    
    Node *current = queue->head;
    while (current) {
        Node *temp = current;
        current = current->next;
        // Note: We don't free the process here as it's owned by the scheduler
        free(temp);
    }
    
    free(queue);
}

// Print the contents of the queue
void print_queue(Queue *queue, const char *name) {
    if (!queue) return;
    
    printf("%s Queue (%d processes):\n", name, queue_size(queue));
    Node *current = queue->head;
    while (current) {
        printf("  P%d (Arrival: %d, State: %d)\n", 
               current->process->pid, 
               current->process->arrival_time,
               current->process->state);
        current = current->next;
    }
}

// Dequeue a specific process from the queue
Process *dequeue_process(Queue *queue, Process *process) {
    if (!queue || !process || !queue->head) return NULL;
    
    Node *current = queue->head;
    Node *prev = NULL;
    
    // Find the node containing the process
    while (current && current->process != process) {
        prev = current;
        current = current->next;
    }
    
    // Process not found
    if (!current) return NULL;
    
    // Remove the node from the queue
    if (prev) {
        prev->next = current->next;
    } else {
        queue->head = current->next;
    }
    
    // Update tail if needed
    if (current == queue->tail) {
        queue->tail = prev;
    }
    
    Process *result = current->process;
    free(current);
    queue->size--;
    
    return result;
}

// Find the process with the shortest next CPU burst (for SJF)
Process *find_shortest_job(Queue *queue) {
    if (!queue || !queue->head) return NULL;
    
    Node *current = queue->head;
    Node *shortest = NULL;
    int shortest_burst = -1;
    
    while (current) {
        // Only consider processes that are ready and have a CPU burst next
        if (current->process->state == READY && 
            current->process->current_burst < current->process->burst_count &&
            current->process->bursts[current->process->current_burst].is_cpu) {
            
            int burst_time = current->process->bursts[current->process->current_burst].duration;
            if (shortest_burst == -1 || burst_time < shortest_burst) {
                shortest = current;
                shortest_burst = burst_time;
            }
        }
        current = current->next;
    }
    
    return shortest ? shortest->process : NULL;
}

// Find the process with the highest priority (for priority scheduling)
Process *find_highest_priority(Queue *queue) {
    if (!queue || !queue->head) return NULL;
    
    Node *current = queue->head;
    Node *highest = NULL;
    int highest_priority = -1;
    
    while (current) {
        // Only consider processes that are ready and have a CPU burst next
        if (current->process->state == READY && 
            current->process->current_burst < current->process->burst_count &&
            current->process->bursts[current->process->current_burst].is_cpu) {
            
            if (highest_priority == -1 || current->process->priority < highest_priority) {
                highest = current;
                highest_priority = current->process->priority;
            }
        }
        current = current->next;
    }
    
    return highest ? highest->process : NULL;
}

// Find the process with the shortest remaining time (for SRTF)
Process *find_shortest_remaining_time(Queue *queue) {
    if (!queue || !queue->head) return NULL;
    
    Node *current = queue->head;
    Process *shortest = current->process;
    int shortest_remaining = (shortest->current_burst < shortest->burst_count) ? 
        shortest->bursts[shortest->current_burst].remaining : INT_MAX;
    
    while (current) {
        Process *p = current->process;
        int remaining = (p->current_burst < p->burst_count) ? 
            p->bursts[p->current_burst].remaining : INT_MAX;
        
        if (remaining < shortest_remaining) {
            shortest_remaining = remaining;
            shortest = p;
        }
        
        current = current->next;
    }
    
    return dequeue_process(queue, shortest);
}

// Remove a specific process from the queue by PID
int remove_process(Queue *queue, int pid) {
    if (!queue || !queue->head) return 0;
    
    Node *prev = NULL;
    Node *current = queue->head;
    
    while (current) {
        if (current->process->pid == pid) {
            if (prev) {
                prev->next = current->next;
            } else {
                queue->head = current->next;
            }
            
            if (current == queue->tail) {
                queue->tail = prev;
            }
            
            free(current);
            queue->size--;
            return 1;
        }
        
        prev = current;
        current = current->next;
    }
    
    return 0;
}
