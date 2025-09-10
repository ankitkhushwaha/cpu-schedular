#include "cpu_thread.h"
#include "dbg.h"
#include "queue.h"
#include "process.h"
#include "thread_op.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void *add_arrival_process(burst_data **data) {
    int j = 0;
    int t_process = (*data)->t_process;
    debug("total process: %d", t_process);
    while (j != t_process) {
        pthread_mutex_lock(&g_counter_mutex);
        if ((*data)->b_data[j]->a_time == global_counter) {
            process_t *p = create_process();
            check(p, "failed to create process");
            p->process_d = (*data)->b_data[j];

            add_to_readyQueue(p);
            add_to_taskList(p);

            sem_post(&full);
            debug("process %d is added to [ready,task_list] queue", j);
            j++;
        }
        global_counter++;
        pthread_mutex_unlock(&g_counter_mutex);
    }
    return NULL;
error:
    exit(EXIT_FAILURE);
}

bool _should_terminate(process_t *pd) {
    if (pd->process_d->cpu_burst_size == 0) {
        // only arrival time is given
        return true;
    }
    if (pd->process_d->io_burst_size == 0) {
        if (pd->cpu_index < pd->process_d->cpu_burst_size) {
            return false;
        }
        return true;
    }

    if ((pd->cpu_index < pd->process_d->cpu_burst_size) &&
        (pd->io_index < pd->process_d->io_burst_size)) {
        return false;
    }
    return true;
}

void *_process_cpu(process_t **pd) {
    check(*pd, "Process given to use is NULL");
    if ((*pd)->status == NEW) {
        if (_should_terminate(*pd)) {
            debug("Process %d will be terminated", (*pd)->pid);
            remove_node_by_pid(readyQueue, (*pd)->pid);
            TERMINATED_PROCESS++;
            (*pd)->status = TEMINATED;
            debug("Process %d is terminated", (*pd)->pid);
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

        return NULL;
    }
    if ((*pd)->status == READY) {
        if (_should_terminate(*pd)) {
            debug("Process %d will be terminated", (*pd)->pid);
            remove_node_by_pid(readyQueue, (*pd)->pid);
            TERMINATED_PROCESS++;
            (*pd)->status = TEMINATED;
            debug("Process %d is terminated", (*pd)->pid);
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

        return NULL;
    }
    debug("Process %d with invalid state: %s given", (*pd)->pid, STATUS_ARR[(*pd)->status]);
    return NULL;
error:
    exit(EXIT_FAILURE);
}

void *_process_io(process_t **pd) {
    check(*pd, "Process given to use is NULL");
    if (_should_terminate(*pd)) {
        debug("Process %d will be terminated", (*pd)->pid);
        remove_node_by_pid(waitQueue, (*pd)->pid);
        TERMINATED_PROCESS++;
        (*pd)->status = TEMINATED;
        debug("Process %d is terminated", (*pd)->pid);
        return NULL;
    }
    if ((*pd)->status == SLEEP) {
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
        
        return NULL;
    }
    debug("Process %d with invalid state: %s given", (*pd)->pid, STATUS_ARR[(*pd)->status]);
    return NULL;
error:
    exit(EXIT_FAILURE);
}

void *schedular() {
    int i = 0;
    while (TERMINATED_PROCESS < 7) {
        sem_wait(&full);
        debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d, index: %d", TERMINATED_PROCESS,
            TOTAL_PROCESS, i);
        pthread_mutex_lock(&readyQueue_mutex);
        running_pd = remove_from_readyQueue();
        if (running_pd) {
            debug("Process %d is doing cpu with state: %s", running_pd->pid, STATUS_ARR[running_pd->status]);
            _process_cpu(&running_pd);
        }
        pthread_mutex_unlock(&readyQueue_mutex);
        i++;
        sem_post(&empty);
    }
    return NULL;
}

void *wake_up() {
    int j = 0;
    // TERMINATED_PROCESS < TOTAL_PROCESS
    while (TERMINATED_PROCESS < 7) {
        sem_wait(&empty);
        debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d, index: %d", TERMINATED_PROCESS,
              TOTAL_PROCESS, j);
        pthread_mutex_lock(&waitQueue_mutex);
        process_t *sleeping_pd = remove_from_waitQueue();
        if (sleeping_pd) {
            debug("Process %d is doing io with state: %s", sleeping_pd->pid, STATUS_ARR[sleeping_pd->status]);
            _process_io(&sleeping_pd);
        }
        pthread_mutex_unlock(&waitQueue_mutex);
        
        j++;
        sem_post(&full);
    }
    return NULL;
}