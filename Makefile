C_FLAGS = -Wall -Isrc -DNDEBUG


SOURCES=$(wildcard src/*.c)
OBJECTS=$(patsubst %.c, %.o, $(SOURCES))

TARGET=build/linklist.o

