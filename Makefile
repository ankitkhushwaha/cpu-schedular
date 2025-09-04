C_FLAGS = -Wall -Isrc -Icpu_scheduling -DNDEBUG


SOURCES=$(wildcard src/*.c cpu_scheduling/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

# TARGET=build/main.o

all: $(OBJECTS) main.c
	$(CC) $(C_FLAGS) $(OBJECTS) main.c -o $(TARGET)
