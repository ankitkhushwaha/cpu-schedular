#ifndef TIME_T_H
#define TIME_T_H

#include <time.h>

typedef struct {
    struct timespec start, end;
} timer__t;

timer__t *create_timer();
struct timespec diff_timespec(timer__t *);
struct timespec timespec_add(struct timespec, struct timespec);
void print_time(struct timespec, char *);

#endif