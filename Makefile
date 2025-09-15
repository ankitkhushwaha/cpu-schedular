CC = gcc
CFLAGS = -Wall -g -Isrc -Ischedule_algorithms -Ischedule_algorithms/fifo  -Ischedule_algorithms/sjf -Ischedule_algorithms/srtf -Ischedule_algorithms/rr -fsanitize=thread
INCLUDES = $(wildcard src/*.h schedule_algorithms/*.h schedule_algorithms/fifo/*.h schedule_algorithms/sjf/*.h schedule_algorithms/srtf/*.h schedule_algorithms/rr/*.h ./*.h)
SOURCES  = $(wildcard src/*.c schedule_algorithms/*.c schedule_algorithms/fifo/*.c schedule_algorithms/sjf/*.c schedule_algorithms/srtf/*.c schedule_algorithms/rr/*.c) main.c
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

