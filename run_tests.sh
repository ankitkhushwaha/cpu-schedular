#!/bin/bash

# Build the project
make clean
make

# Test with different scheduling algorithms
echo "\n=== Testing FCFS Scheduling ==="
./bin/cpu_scheduler fcfs test_cases/test1.dat

echo "\n=== Testing SJF (Non-preemptive) Scheduling ==="
./bin/cpu_scheduler sjf test_cases/test1.dat

echo "\n=== Testing SRTF (Preemptive SJF) Scheduling ==="
./bin/cpu_scheduler srtf test_cases/test1.dat

echo "\n=== Testing Priority Scheduling ==="
./bin/cpu_scheduler priority test_cases/test1.dat

echo "\n=== Testing Round Robin (time quantum=2) Scheduling ==="
./bin/cpu_scheduler rr test_cases/test1.dat 2
