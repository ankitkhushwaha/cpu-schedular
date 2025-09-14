#ifndef SCHED_COMMON_H
#define SCHED_COMMON_H

#include "file.h"
#include "process.h"
#include "time_t.h"

extern timer__t *wall_timer;
extern int start_time, end_time;
extern void print_usage(const char *prog_name);

typedef struct {
    void *(*scheduler_core)(void);
    void *(*wakeUp_core)(void);
    void *(*add_arrival_core)(burst_data **);
} scheduler_ops_t;

scheduler_ops_t *create_scheduler();
void init_scheduler(char *sched_type);
void destroy_scheduler(scheduler_ops_t *);

#endif