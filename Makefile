CC = gcc
CFLAGS = -Wall -g -Isrc -Icpu_scheduling -fsanitize=thread
INCLUDES = $(wildcard src/*.h cpu_scheduling/*.h ./*.h)
SOURCES  = $(wildcard src/*.c cpu_scheduling/*.c) main.c
OBJECTS  = $(patsubst %.c, %.o, $(SOURCES))

TARGET = main
# -fsanitize=thread
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

