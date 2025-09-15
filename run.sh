#!/bin/bash

files=(test_cases/*)
algos=(fcfs sjf srtf rr)

make -j8
clear

echo "Running all test cases"

for i in "${files[@]}"; do
    for j in "${algos[@]}"; do
        echo "Running $j on $i"
        ./main "$j" "$i"
        echo ""
    done
done
