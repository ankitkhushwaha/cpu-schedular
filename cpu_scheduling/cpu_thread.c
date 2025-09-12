#include "cpu_thread.h"
#include "dbg.h"
#include "process.h"
#include "queue.h"
#include "thread_op.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

atomic_int wait_counter = 0;
atomic_int ready_counter = 0;

void *add_arrival_process(burst_data **data) {
    int j = 0;
    int t_process = (*data)->t_process;
    debug("total process: %d", t_process);
    while (j != t_process) {
        if ((*data)->b_data[j]->a_time <= read_global_counter()) {
            process_t *p = create_process();
            check(p, "failed to create process");
            p->process_d = (*data)->b_data[j];

            add_to_readyQueue(p);
            add_to_taskList(p);
            debug("process %d is added to [ready,task_list] queue at time: %d", j,
                  read_global_counter());
            sem_post(&ready_count);
            debug("READY COUNT:%d POSTED from arrival.", atomic_load(&ready_counter));
            j++;
            update_global_counter(1);
        }
    }
    fprintf(stderr, "arrival thread exiting\n");
    return NULL;
error:
    exit(EXIT_FAILURE);
}

static bool _should_terminate(process_t *pd) {
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

    if (((pd->cpu_index < pd->process_d->cpu_burst_size) &&
         (pd->io_index <= pd->process_d->io_burst_size)) ||

        ((pd->cpu_index <= pd->process_d->cpu_burst_size) &&
         (pd->io_index < pd->process_d->io_burst_size))) {
        return false;
    }
    return true;
}

static STATUS _process_cpu(process_t **pd) {
    check(*pd, "Process given to use is NULL");
    if (_should_terminate(*pd)) {
        debug("Process %d will be terminated", (*pd)->pid);
        update_term_counter(1);
        (*pd)->status = TERMINATED;
        debug("Process %d is terminated", (*pd)->pid);
        return TERMINATED;
    }

    if ((*pd)->status == NEW) {
        (*pd)->cpu_time = (*pd)->process_d->cpu_burst[0];
        update_global_counter((*pd)->process_d->cpu_burst[0]);
        (*pd)->cpu_index += 1;

        (*pd)->status = SLEEP;
        add_to_waitQueue(*pd);
        sem_post(&wait_count);
        return SLEEP;
    }
    if ((*pd)->status == READY) {
        // cpu work will be done
        (*pd)->cpu_time += (*pd)->process_d->cpu_burst[(*pd)->cpu_index];
        update_global_counter((*pd)->process_d->cpu_burst[(*pd)->cpu_index]);
        (*pd)->cpu_index += 1;

        (*pd)->status = SLEEP;
        add_to_waitQueue(*pd);
        sem_post(&wait_count);
        return SLEEP;
    }
    debug("Process %d with invalid state: %s given", (*pd)->pid, STATUS_ARR[(*pd)->status]);
    return UNDEFINED;
error:
    exit(EXIT_FAILURE);
}

static STATUS _process_io(process_t **pd) {
    check(*pd, "Process given to use is NULL");
    if (_should_terminate(*pd)) {
        debug("Process %d will be terminated", (*pd)->pid);
        update_term_counter(1);
        (*pd)->status = TERMINATED;
        debug("Process %d is terminated", (*pd)->pid);
        return TERMINATED;
    }
    if ((*pd)->status == SLEEP) {
        // io work will be done
        (*pd)->io_time += (*pd)->process_d->io_burst[(*pd)->io_index];
        update_global_counter((*pd)->process_d->io_burst[(*pd)->io_index]);
        (*pd)->io_index += 1;

        (*pd)->status = READY;
        add_to_readyQueue(*pd);
        sem_post(&ready_count);
        return READY;
    }
    debug("Process %d with invalid state: %s given", (*pd)->pid, STATUS_ARR[(*pd)->status]);
    return UNDEFINED;
error:
    exit(EXIT_FAILURE);
}

void *schedular() {
    int i = 0;
    // int desired = TOTAL_PROCESS;
    while (1) {
        i++;
        // atomic_compare_exchange_strong(&TERMINATED_PROCESS, &TOTAL_PROCESS, &desired;)
        // Check if all processes are done
        // if (all_processes_done()) {
        //     debug("All processes completed. Exiting scheduler.");
        //     break;
        // }

        // Try to get a process from ready queue with a small timeout
        // struct timespec ts;
        // clock_gettime(CLOCK_REALTIME, &ts);
        // ts.tv_sec += 1; // 1 second timeout

        // int sem_ret = sem_timedwait(&ready_count, &ts);
        // if (sem_ret == -1) {
        //     if (errno == ETIMEDOUT) {
        //         // Check again if all processes are done
        //         // if (all_processes_done()) {
        //         //     debug("All processes completed. Exiting scheduler.");
        //         //     break;
        //         // }
        //         continue;
        //     }
        //     perror("sem_timedwait");
        //     break;
        // }
        if (atomic_load(&TERMINATED_PROCESS) >= TOTAL_PROCESS) {
            debug("BReaking TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d",
                  atomic_load(&TERMINATED_PROCESS), TOTAL_PROCESS);
            break;
        } else {
            debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d", atomic_load(&TERMINATED_PROCESS),
                  TOTAL_PROCESS);
        }

        if (all_processes_done()) {
            debug("Ready and wait queue is empty");
            break;
        }
        if (sem_trywait(&ready_count) == 0) {
            debug("READY COUNT WAITED Schedular");
            debug("READY COUNT:%d, wait count: %d", atomic_load(&ready_counter),
                  atomic_load(&wait_counter));
            // debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d, index: %d",
            //      read_term_counter(), TOTAL_PROCESS, i);

            running_pd = remove_from_readyQueue();
            if (running_pd) {
                debug("Process %d is doing cpu with state: %s", running_pd->pid,
                      STATUS_ARR[running_pd->status]);
                STATUS st = _process_cpu(&running_pd);
                if (st == SLEEP) {
                    // sem_post(&wait_count);
                    // atomic_fetch_add(&wait_counter, 1);
                    debug("READY COUNT:%d, wait count: %d", atomic_load(&ready_counter),
                          atomic_load(&wait_counter));
                }
                if (st == TERMINATED) {
                    debug("TERMINATED process: %d ", atomic_load(&TERMINATED_PROCESS));
                }
            } else {
                debug("BUG: No process in ready queue");
            }
        }
    }
    debug("Scheduler exiting\n");
    return NULL;
}

void *wake_up() {
    int j = 0;
    while (atomic_load(&TERMINATED_PROCESS) < TOTAL_PROCESS) {
        debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d", atomic_load(&TERMINATED_PROCESS),
              TOTAL_PROCESS);
        // Check if all processes are done
        // if (all_processes_done()) {
        //     debug("All processes completed. Exiting wake_up.");
        //     break;
        // if (atomic_load(&TERMINATED_PROCESS) >= TOTAL_PROCESS)
        //     break;
        // // Try to get a process from wait queue with a small timeout
        // struct timespec ts;
        // clock_gettime(CLOCK_REALTIME, &ts);
        // ts.tv_sec += 1; // 1 second timeout

        // int sem_ret = sem_timedwait(&wait_count, &ts);
        // if (sem_ret == -1) {
        //     if (errno == ETIMEDOUT) {
        //         // Check again if all processes are done
        //         // if (all_processes_done()) {
        //         //     debug("All processes completed. Exiting wake_up.");
        //         //     break;
        //         // }
        //         continue;
        //     }
        //     perror("sem_timedwait");
        //     break;
        // }
        // }
        // if (!is_emptyWaitQueue_t() && is_emptyReadyQueue_t()) {
        //     debug("wait Queue is empty");
        //     continue;
        // }
        if (sem_trywait(&wait_count) == 0) {
            // debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d, index: %d",
            sleeping_pd = remove_from_waitQueue();
            if (sleeping_pd) {
                debug("Process %d is doing io with state: %s", sleeping_pd->pid,
                      STATUS_ARR[sleeping_pd->status]);
                STATUS st = _process_io(&sleeping_pd);
                if (st == READY) {
                    // sem_post(&ready_count);
                    // atomic_fetch_add(&ready_counter, 1);
                    debug("READY COUNT:%d, wait count: %d", atomic_load(&ready_counter),
                          atomic_load(&wait_counter));
                }
                if (st == TERMINATED) {
                    debug("TERMINATED process: %d ", atomic_load(&TERMINATED_PROCESS));
                }
            } else {
                debug("BUG: No process in wait queue");
            }
        }
        //      read_term_counter(), TOTAL_PROCESS, j);

        j++;
    }
    debug("wakeup exiting\n");
    return NULL;
}