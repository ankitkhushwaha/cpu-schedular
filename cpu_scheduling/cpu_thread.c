#include <stdio.h>
#include <pthread.h>
#include "cpu_thread.h"
#include "thread_op.h"

extern pthread_mutex_t g_counter_mutex;
extern int global_counter;

void *arrivalQueue(burst_data *data){
    int j=0;
    while (1){
        pthread_mutex_lock(&g_counter_mutex);
        global_counter++;
        pthread_mutex_unlock(&g_counter_mutex);
        if (data->b_data[j]->a_time == global_counter){
            add_to_readyQueue(data->b_data[j]);
            j++;
        }
    }
}
