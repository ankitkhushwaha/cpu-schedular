#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NCPU 2

typedef struct {
    int id;
    int burst_time;
} process_t;

pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

process_t *ready_queue[100];
int front = 0, rear = 0;

pthread_mutex_t cpu_lock[NCPU];   // one lock per CPU slot
process_t *cpu_task[NCPU] = {NULL};  // current process per CPU

// Enqueue new process
void enqueue(process_t *p) {
    pthread_mutex_lock(&queue_lock);
    ready_queue[rear++] = p;
    pthread_cond_signal(&queue_not_empty);
    pthread_mutex_unlock(&queue_lock);
}

// Dequeue process
process_t* dequeue() {
    process_t *p = NULL;
    if (front < rear) {
        p = ready_queue[front++];
    }
    return p;
}

// Worker (CPU thread)
void *cpu_worker(void *arg) {
    int cpu_id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&queue_lock);
        while (front == rear) { // wait if queue empty
            pthread_cond_wait(&queue_not_empty, &queue_lock);
        }
        process_t *p = dequeue();
        pthread_mutex_unlock(&queue_lock);

        if (p) {
            pthread_mutex_lock(&cpu_lock[cpu_id]);
            cpu_task[cpu_id] = p;
            pthread_mutex_unlock(&cpu_lock[cpu_id]);

            printf("CPU%d running process %d (burst=%d)\n", cpu_id, p->id, p->burst_time);
            sleep(p->burst_time);  // simulate CPU burst
            printf("CPU%d finished process %d\n", cpu_id, p->id);

            pthread_mutex_lock(&cpu_lock[cpu_id]);
            cpu_task[cpu_id] = NULL; // mark free
            pthread_mutex_unlock(&cpu_lock[cpu_id]);

            free(p);
        }
    }
    return NULL;
}

int main() {
    pthread_t cpus[NCPU];
    int cpu_ids[NCPU];

    for (int i = 0; i < NCPU; i++) {
        cpu_ids[i] = i;
        pthread_mutex_init(&cpu_lock[i], NULL);
        pthread_create(&cpus[i], NULL, cpu_worker, &cpu_ids[i]);
    }

    // Add some processes
    for (int i = 0; i < 5; i++) {
        process_t *p = malloc(sizeof(process_t));
        p->id = i;
        p->burst_time = (i % 3) + 1;  // 1–3 seconds
        enqueue(p);
        sleep(1); // stagger arrival
    }

    pthread_exit(NULL);
}
