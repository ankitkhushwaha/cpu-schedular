#include <stdio.h>
#include <pthread.h>
#include "dbg.h"
#include "cpu_thread.h"
#include "thread_op.h"

void *add_arrival_process(burst_data **data)
{
    int j = 0;
    int t_process = (*data)->t_process;
    debug("total process: %d", t_process);
    while (j != t_process)
    {
        pthread_mutex_lock(&g_counter_mutex);
        if ((*data)->b_data[j]->a_time == global_counter)
        {
            add_to_readyQueue((*data)->b_data[j]);
            sem_post(&full);
            debug("process %d is added to ready queue", j);
            j++;
        }
        global_counter++;
        pthread_mutex_unlock(&g_counter_mutex);
    }
    return NULL;
}

void *schedular()
{
    while (1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&readyQueue_mutex);
        
        pthread_mutex_unlock(&readyQueue_mutex);
    }
}
