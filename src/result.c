#include "process.h"
#include "result.h"
#include "time_t.h"
#include <time.h>

void write_result(){
    int t_avg = 0, t_max = 0;
    struct timespec ts = {0, 0};

    node *tnode;
    Traverse(tnode, task_list) {
        ts = timespec_add(ts, tnode->data->wall_time);
        t_avg += tnode->data->turnaround_time;
        t_max = tnode->data->turnaround_time > t_max ? tnode->data->turnaround_time : t_max;
    }
    // iternate through last one
    t_avg += tnode->data->turnaround_time;
    t_max = tnode->data->turnaround_time > t_max ? tnode->data->turnaround_time : t_max;

    t_avg /= TOTAL_PROCESS;

    printf("Turnaround Average: %d, Turnaround Maximum: %d\n",t_avg, t_max);
    print_time(ts, "Wall Time", false);
}