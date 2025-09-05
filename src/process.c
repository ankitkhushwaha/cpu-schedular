#include <stdio.h>
#include <stdlib.h>
#include "process.h"

process_t *create_process(){
    process_t *process = (process_t *)calloc(1, sizeof(process_t));
    process->pid = TOTAL_PROCESS;
    TOTAL_PROCESS++;
    return process;
}