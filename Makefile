CFLAGS = -Wall -g -Isrc -Icpu_scheduling 


SOURCES=$(wildcard src/*.c cpu_scheduling/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

# %.o: %.c
# 	$(CC) $(CFLAGS) -c -o $@ $<

# TARGET=build/main.o

all: 
	$(CC) $(CFLAGS) $(OBJECTS) main.c -o main

gdb:
	gdb --args ./main test_cases/process1.dat
