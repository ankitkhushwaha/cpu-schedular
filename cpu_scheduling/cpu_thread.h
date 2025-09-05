#ifndef CPU_THREAD_H
#define CPU_THREAD_H

#include "queue.h"
#include "file.h"

Queue *schedular(Queue **);
void *arrivalQueue(burst_data **);

#endif