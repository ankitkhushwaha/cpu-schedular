#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "file.h"
#include "thread_op.h"
#include "cpu_thread.h"

Queue *readyQueue;
Queue *waitQueue;

extern burst_data * read_burstfile(char *);
extern void *init_queues();
extern void arrivalQueue(burst_data *process_t);

sem_t empty;
sem_t full;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]){
    char *algo = argv[1];
    burst_data *data = read_burstfile(argv[2]);
    
    init_queues();
    
    pthread_t arrivalThread;
    pthread_t cpuThread;
    pthread_t ioThread;

    sem_init(&empty, 0, data->t_process); 
    sem_init(&full, 0, 0);        

    pthread_create(&arrivalThread, NULL, arrivalQueue, data);
    
    return 0;
}