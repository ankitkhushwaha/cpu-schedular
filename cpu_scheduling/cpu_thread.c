#include "cpu_thread.h"
#include "dbg.h"
#include "process.h"
#include "queue.h"
#include "thread_op.h"
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "time_t.h"

timer__t *wall_timer;
int start_time, end_time;

void *add_arrival_process(burst_data **data) {
    int j = 0;
    int t_process = (*data)->t_process;
    int max_arrTime = (*data)->b_data[t_process - 1]->a_time;
    debug("total process: %d, MAX ARRIVAL TIME: %d", t_process, max_arrTime);
    while (j != t_process) {
        if ((*data)->b_data[j]->a_time <= read_global_counter()) {
            process_t *p = create_process();
            check(p, "failed to create process");
            p->process_d = (*data)->b_data[j];
            
            assert_t((*data)->b_data[j]->a_time == read_global_counter());
            p->process_d->a_time = (*data)->b_data[j]->a_time;
            
            add_to_readyQueue(p);
            add_to_taskList(p);
            debug("process %d is added to [ready,task_list] queue at time: %d", j,
                  read_global_counter());
            sem_post(&ready_count);
            j++;
        }
        debug("global counter: %d, max_arrTime: %d", read_global_counter(), max_arrTime);
        assert_t(read_global_counter() <= max_arrTime);
        if (j == t_process)
            break;
        update_global_counter(1);
    }
    debug("j: %d, t_process: %d", j, t_process);
    assert_t(j == t_process);
    debug("arrival thread exiting");
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
    
    STATUS _status;
    clock_gettime(CLOCK_REALTIME, &wall_timer->start);
    print_time(wall_timer->start, "START", true);
    if (_should_terminate(*pd)) {
        debug("Process %d will be terminated", (*pd)->pid);
        update_term_counter(1);
        (*pd)->status = TERMINATED;

        debug("process time: %d, arrival time: %d, global_time: %d", 
            (*pd)->process_time, (*pd)->process_d->a_time, read_global_counter());
        (*pd)->turnaround_time = (*pd)->process_time;
        assert_t((*pd)->turnaround_time >= 0);
        debug("turnaround time: %d", (*pd)->turnaround_time);
        debug("Process %d is terminated", (*pd)->pid);
        
        _status =  TERMINATED;
        goto final;
    }
    
    if ((*pd)->status == NEW) {
        start_time = read_global_counter();
        (*pd)->cpu_time = (*pd)->process_d->cpu_burst[0];
        update_global_counter((*pd)->process_d->cpu_burst[0]);
        end_time = read_global_counter();
        (*pd)->process_time += (*pd)->process_d->cpu_burst[0];
        
        (*pd)->cpu_index += 1;
        (*pd)->status = SLEEP;
        add_to_waitQueue(*pd);
        sem_post(&wait_count);
        _status = SLEEP;
        goto final;
    }
    if ((*pd)->status == READY) {
        start_time = read_global_counter();
        // cpu work will be done
        (*pd)->cpu_time += (*pd)->process_d->cpu_burst[(*pd)->cpu_index];
        (*pd)->process_time += (*pd)->process_d->cpu_burst[(*pd)->cpu_index];

        update_global_counter((*pd)->process_d->cpu_burst[(*pd)->cpu_index]);
        end_time = read_global_counter();
        (*pd)->cpu_index += 1;
        
        (*pd)->status = SLEEP;
        add_to_waitQueue(*pd);
        sem_post(&wait_count);
        _status = SLEEP;
        goto final;
    }
    debug("Process %d with invalid state: %s given", (*pd)->pid, STATUS_ARR[(*pd)->status]);
    _status = UNDEFINED;

final:
    if (_status!= TERMINATED && _status!= UNDEFINED)
        write_cpu_process_data(*pd, start_time, end_time);
    clock_gettime(CLOCK_REALTIME, &wall_timer->end);
    print_time(wall_timer->end, "END", true);
    struct timespec diff = diff_timespec(wall_timer);
    print_time(diff, "diff_Time", true);
    (*pd)->wall_time = timespec_add((*pd)->wall_time, diff);
    return _status;

error:
    exit(EXIT_FAILURE);
}

static STATUS _process_io(process_t **pd) {
    check(*pd, "Process given to use is NULL");
    if (_should_terminate(*pd)) {
        debug("Process %d will be terminated", (*pd)->pid);
        update_term_counter(1);
        (*pd)->status = TERMINATED;

        debug("cpu usage time: %d, arrival time: %d, global_time: %d", 
            (*pd)->process_time, (*pd)->process_d->a_time, read_global_counter());
        (*pd)->turnaround_time = (*pd)->process_time;
        assert_t((*pd)->turnaround_time >= 0);
        debug("turnaround time: %d", (*pd)->turnaround_time);

        debug("Process %d is terminated", (*pd)->pid);
        return TERMINATED;
    }
    if ((*pd)->status == SLEEP) {
        // io work will be done
        (*pd)->io_time += (*pd)->process_d->io_burst[(*pd)->io_index];
        update_global_counter((*pd)->process_d->io_burst[(*pd)->io_index]);
        (*pd)->process_time += (*pd)->process_d->io_burst[(*pd)->io_index];
        
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
    wall_timer = create_timer();
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
            // debug("TERMINATED_PROCESS: %d, TOTAL_PROCESS: %d, index: %d",
            //      read_term_counter(), TOTAL_PROCESS, i);

            running_pd = remove_from_readyQueue();
            if (running_pd) {
                debug("Process %d is doing cpu with state: %s", running_pd->pid,
                      STATUS_ARR[running_pd->status]);
                STATUS st = _process_cpu(&running_pd);
                if (st == SLEEP) {
                    // sem_post(&wait_count);
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
    free(wall_timer);
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