CFLAGS = -Wall -g -Isrc -Icpu_scheduling 

INCLUDES=$(wildcard src/*.h cpu_scheduling/*.h ./*.h)
SOURCES=$(wildcard src/*.c cpu_scheduling/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

TEST_OBJECTS=$(wildcard src/*.o cpu_scheduling/*.o)
# %.o: %.c
# 	$(CC) $(CFLAGS) -c -o $@ $<

# TARGET=build/main.o

all: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) main.c -o main

gdb: all
	gdb --args ./main test_cases/process1.dat

clean: $(TEST_OBJECTS)
	rm -rf $(TEST_OBJECTS)

format: $(INCLUDES) $(SOURCES)
	clang-format -i $(INCLUDES) $(SOURCES)
