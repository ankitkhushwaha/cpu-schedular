CFLAGS = -Wall -Isrc -Icpu_scheduling -DNDEBUG


SOURCES=$(wildcard src/*.c cpu_scheduling/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# TARGET=build/main.o

all: $(OBJECTS) main.c
	$(CC) $(CFLAGS) $(OBJECTS) main.c -o main
