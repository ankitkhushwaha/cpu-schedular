#include <stdio.h>
#include <stdbool.h>
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
            process_t *p = create_process();
            check(p, "failed to create process");
            p->process_d = (*data)->b_data[j];

            add_to_readyQueue(p);
            add_to_taskList(p);

            sem_post(&full);
            debug("process %d is added to ready queue", j);
            j++;
        }
        global_counter++;
        pthread_mutex_unlock(&g_counter_mutex);
    }
    return NULL;
error:
    return NULL;
}

bool _should_terminate(process_t *pd)
{
    if (pd->process_d->cpu_burst_size == 0)
    {
        // only arrival time is given
        return true;
    }
    if (pd->process_d->io_burst_size == 0)
    {
        if (pd->cpu_index < pd->process_d->cpu_burst_size)
        {
            return false;
        }
        return true;
    }

    if ((pd->cpu_index < pd->process_d->cpu_burst_size) &&
        (pd->io_index < pd->process_d->io_burst_size))
    {
        return false;
    }
    return true;
}

void *_process_cpu(process_t **pd)
{
    check(*pd, "Process given to use is NULL");
    if ((*pd)->status == NEW)
    {
        if (_should_terminate(*pd))
        {
            (*pd)->status = TEMINATED;
            return NULL;
        }
        pthread_mutex_lock(&g_counter_mutex);
        (*pd)->cpu_time = (*pd)->process_d->cpu_burst[0];
        global_counter += (*pd)->process_d->cpu_burst[0];
        (*pd)->cpu_index += 1;
        pthread_mutex_unlock(&g_counter_mutex);

        pthread_mutex_lock(&waitQueue_mutex);
        (*pd)->status = SLEEP;
        add_to_waitQueue(*pd);
        pthread_mutex_unlock(&waitQueue_mutex);
    }
    if ((*pd)->status == READY)
    {
        if (_should_terminate(*pd))
        {
            (*pd)->status = TEMINATED;
            return NULL;
        }
        // cpu work will be done
        pthread_mutex_lock(&g_counter_mutex);
        (*pd)->cpu_time += (*pd)->process_d->cpu_burst[(*pd)->cpu_index];
        global_counter += (*pd)->process_d->cpu_burst[(*pd)->cpu_index];
        (*pd)->cpu_index += 1;
        pthread_mutex_unlock(&g_counter_mutex);

        pthread_mutex_lock(&waitQueue_mutex);
        (*pd)->status = SLEEP;
        add_to_waitQueue(*pd);
        pthread_mutex_unlock(&waitQueue_mutex);
    }
error:
    return NULL;
}

void *_process_io(process_t **pd)
{
    check(*pd, "Process given to use is NULL");
    if (_should_terminate(*pd))
    {
        (*pd)->status = TEMINATED;

        return NULL;
    }
    // io work will be done
    pthread_mutex_lock(&g_counter_mutex);
    (*pd)->io_time += (*pd)->process_d->io_burst[(*pd)->io_index];
    global_counter += (*pd)->process_d->io_burst[(*pd)->io_index];
    (*pd)->io_index += 1;
    pthread_mutex_unlock(&g_counter_mutex);

    pthread_mutex_lock(&readyQueue_mutex);
    (*pd)->status = READY;
    add_to_readyQueue(*pd);
    pthread_mutex_unlock(&readyQueue_mutex);

error:
    return NULL;
}

void *schedular()
{
    while (TOTAL_PROCESS <= TERMINATED_PROCESS)
    {
        sem_wait(&empty);
        pthread_mutex_lock(&readyQueue_mutex);
        running_pd = remove_from_readyQueue();
        _process_cpu(&running_pd);
        pthread_mutex_unlock(&readyQueue_mutex);
        sem_post(&full);
    }
    return NULL;
}

void *wake_up()
{
    while (TOTAL_PROCESS <= TERMINATED_PROCESS)
    {
        sem_wait(&full);
        pthread_mutex_lock(&waitQueue_mutex);
        process_t *sleeping_pd = remove_from_waitQueue();
        _process_io(&sleeping_pd);
        pthread_mutex_unlock(&waitQueue_mutex);
        sem_post(&empty);
    }
    return NULL;
}