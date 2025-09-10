#include "process.h"
#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>

const char *STATUS_ARR[5] = {"NEW", "READY", "SLEEP", "RUNNING", "TERMINATED"};

process_t *create_process() {
    process_t *process = (process_t *)calloc(1, sizeof(process_t));
    check_mem(process);
    process->pid = T_PROCESS;
    process->status = NEW;
    T_PROCESS++;
    return process;
error:
    return NULL;
}