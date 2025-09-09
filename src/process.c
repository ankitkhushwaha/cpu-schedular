#include "process.h"
#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>

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