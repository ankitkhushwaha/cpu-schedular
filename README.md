# CPU Scheduler Simulation

This project implements a **multithreaded CPU scheduling simulator** in C.
It demonstrates how different scheduling policies affect turnaround time, waiting time, and execution order of processes.

## Features

* **Multithreaded simulation single core**

  * **Arrival Thread** – simulates process arrivals.
  * **Scheduler Thread** – selects processes for CPU execution.
  * **Wakeup Thread** – handles I/O-completed processes returning to the ready queue.

* **Supported Scheduling Algorithms**

  * **First Come First Serve (FCFS)** – Non-preemptive, processes run in arrival order.
  * **Shortest Job First (SJF)** – Non-preemptive, picks process with shortest burst time.
  * **Shortest Remaining Time First (SRTF)** – Preemptive, dynamically selects process with least remaining burst.
  * **Round Robin (RR)** – Preemptive, each process runs for a fixed time quantum before requeue.

* **Metrics Collected**

  * Average turnaround time
  * Maximum completion time
  * Simulator run time

## Requirements

* GCC / Clang
* GNU Make
* pthread library (POSIX threads)

### Tested on
* gcc version 15.2.1 (GCC)
* GNU Make 4.4.1
* Fedora 42

## Build & Run

```bash
git clone <repo_url>
cd cpu_scheduler
make
./main <input_file> <algorithm>
```

Example:

```bash
./main test_cases/process1.dat rr 
```

## Output

The simulator prints:

* Console Output

  * Turnaround time for each process
  * Waiting time for each process
  * Average turnaround and waiting times per scheduling policy

* Log File

  * Timeline of CPU execution
  * Which process ran, at what time, and for how long
  * Context switch points (when preemption or quantum expiry occurs)

```
CPU0
<process_name>,<cpu_id>    <start_time>    <end_time>
p0,1    40    55
p1,1    55    105
p2,1    105    120
p3,1    120    172
```

## Limitations

* Currently supports only a single CPU core.
* I/O support is limited.
* Memory management is minimal (possible leaks).
* Code can be refactored for cleaner design and extensibility.

## License

[MIT License](LICENSE) – feel free to modify and use.
