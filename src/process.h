#ifndef PROCESS_H
#define PROCESS_H

extern int TOTAL_PROCESS;

typedef struct {
    int pid;
}process_t;

process_t *create_process();
#endif