set debuginfod enabled on
display *readyQueue
display *waitQueue
b cpu_scheduling/cpu_thread.c:139 if i == 6
r