# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Isrc -DNDEBUG -pthread
LDFLAGS = -pthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC_FILES))

# Executable
TARGET = $(BIN_DIR)/cpu_scheduler

# Default target
all: $(TARGET)

# Create necessary directories
$(shell mkdir -p $(BIN_DIR) $(OBJ_DIR))

# Link the executable
$(TARGET): $(OBJ_FILES)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Dependencies
$(OBJ_DIR)/scheduler.o: $(INCLUDE_DIR)/scheduler.h $(INCLUDE_DIR)/process.h $(INCLUDE_DIR)/queue.h
$(OBJ_DIR)/process.o: $(INCLUDE_DIR)/process.h
$(OBJ_DIR)/process_queue.o: $(INCLUDE_DIR)/queue.h $(INCLUDE_DIR)/process.h
$(OBJ_DIR)/file_parser.o: $(INCLUDE_DIR)/process.h

# Clean build artifacts
clean:
	rm -rf $(BIN_DIR) $(OBJ_DIR)

# Run the program with default arguments
run: $(TARGET)
	@$(TARGET) fcfs test_cases/process1.dat

# Run with valgrind for memory checking
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all $(TARGET) fcfs test_cases/process1.dat

.PHONY: all clean run valgrind
