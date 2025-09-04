#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

// Node structure for the queue
typedef struct Node {
    Process *process;
    struct Node *next;
} Node;

// Queue structure
typedef struct {
    Node *head;
    Node *tail;
    int size;
} Queue;

// Basic Queue Operations
Queue *create_queue(void);
void enqueue(Queue *queue, Process *process);
Process *dequeue(Queue *queue);
Process *peek(Queue *queue);
int is_empty(Queue *queue);
int queue_size(Queue *queue);
void free_queue(Queue *queue);
void print_queue(Queue *queue, const char *name);

// Specialized Queue Operations for Scheduling
Process *dequeue_process(Queue *queue, Process *process);
Process *find_shortest_job(Queue *queue);
Process *find_highest_priority(Queue *queue);
Process *find_shortest_remaining_time(Queue *queue);
int remove_process(Queue *queue, int pid);
void sort_queue_by_arrival(Queue *queue);
void sort_queue_by_burst(Queue *queue);
void sort_queue_by_priority(Queue *queue);

// Debugging Utilities
void print_process_list(Process **processes, int count);

#endif // QUEUE_H
