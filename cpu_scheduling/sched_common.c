#include "sched_common.h"
#include "dbg.h"
#include "fifo_sched.h"
#include "sjf_sched.h"
#include "sjf_th.h"
#include "fifo_th.h"
#include <stdlib.h>

timer__t *wall_timer;
int start_time = 0, end_time = 0;

scheduler_ops_t *create_scheduler() {
    scheduler_ops_t *sched = calloc(1, sizeof(scheduler_ops_t));
    check_mem(sched);
    return sched;
error:
    exit(EXIT_FAILURE);
}

void init_scheduler(char *sched_type) {
    if (strcmp(sched_type, "fcfs") == 0) {
        init_fcfs_sched();
        init_fcfs_th();
    }
    else if (strcmp(sched_type, "sjf") == 0) {
        init_sjf();
        init_sjf_th();
    }
    // else if (strcmp(sched_type, "priority") == 0) {
    //     init_priority();
    // } else if (strcmp(sched_type, "rr") == 0) {
    //     init_rr();
    // }
    else {
        printf("Error: Unknown scheduler '%s'\n\n", sched_type);
        print_usage("./cpu_scheduler");
        exit(1);
    }
}
