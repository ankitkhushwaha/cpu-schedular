#include "helper.h"
#include "dbg.h"
#include "file.h"
#include "process.h"
#include "thread_op.h"
#include <stdlib.h>

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

            debug("process %d atime: %d with global counter: %d", j, (*data)->b_data[j]->a_time,
                  read_global_counter());
            // assert_t((*data)->b_data[j]->a_time == read_global_counter());
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
