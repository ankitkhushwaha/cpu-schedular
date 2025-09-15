CC = gcc
CFLAGS = -Wall -g -Isrc -Icpu_scheduling -Icpu_scheduling/fifo  -Icpu_scheduling/sjf -Icpu_scheduling/srtf -Icpu_scheduling/rr -fsanitize=thread
INCLUDES = $(wildcard src/*.h cpu_scheduling/*.h cpu_scheduling/fifo/*.h cpu_scheduling/sjf/*.h cpu_scheduling/srtf/*.h cpu_scheduling/rr/*.h ./*.h)
SOURCES  = $(wildcard src/*.c cpu_scheduling/*.c cpu_scheduling/fifo/*.c cpu_scheduling/sjf/*.c cpu_scheduling/srtf/*.c cpu_scheduling/rr/*.c) main.c
OBJECTS  = $(patsubst %.c, %.o, $(SOURCES))

TARGET = main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

gdb: $(TARGET)
	gdb --args ./$(TARGET) test_cases/process1.dat $(CORE)

run: $(TARGET)
	TSAN_OPTIONS=second_deadlock_stack=1 ./$(TARGET) test_cases/process1.dat

clean:
	rm -f $(OBJECTS) $(TARGET) a.out

format:
	clang-format -i $(INCLUDES) $(SOURCES) 

