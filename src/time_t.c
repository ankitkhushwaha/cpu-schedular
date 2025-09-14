#include "time_t.h"
#include "dbg.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

timer__t *create_timer() {
    timer__t *timer = (timer__t *)calloc(1, sizeof(timer__t));
    check_mem(timer);
    return timer;
error:
    exit(EXIT_FAILURE);
}

struct timespec diff_timespec(timer__t *timer) {
    struct timespec diff;
    diff.tv_sec = timer->end.tv_sec - timer->start.tv_sec;
    diff.tv_nsec = timer->end.tv_nsec - timer->start.tv_nsec;

    if (diff.tv_nsec < 0) {
        diff.tv_nsec += 1000000000;
        diff.tv_sec--;
    }
    return diff;
}

struct timespec timespec_add(struct timespec t1, struct timespec t2) {
    struct timespec result;

    result.tv_sec = t1.tv_sec + t2.tv_sec;
    result.tv_nsec = t1.tv_nsec + t2.tv_nsec;

    if (result.tv_nsec >= 1000000000L) {
        result.tv_nsec -= 1000000000L;
        result.tv_sec += 1;
    }

    return result;
}

void print_time(struct timespec t, char *clockType, bool res) {
    if (res)
        debug("%s: %ld.%09ld\n", clockType, t.tv_sec, t.tv_nsec);
    else
        printf("%s: %ld.%09ld\n", clockType, t.tv_sec, t.tv_nsec);
}