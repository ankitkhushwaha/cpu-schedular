#include <stdio.h>
#include <pthread.h>
#include "cpu_thread.h"
#include "thread_op.h"

extern pthread_mutex_t mutex;

int global_counter = 0;
void arrivalQueue(burst_data *process_t){
    int j=0;
    while (1){
        pthread_mutex_lock(&mutex);
        global_counter++;
        pthread_mutex_unlock(&mutex);
        if (process_t->b_data[j]->a_time == global_counter){
            add_to_readyQueue(process_t);
            j++;
        }
    }
}
