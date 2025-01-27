# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = 

# Directories
SRC_DIR = Ex1 Ex2
BIN_DIR = ./bin

# Entry points
ENTRY_POINTS = Ex1/a.c Ex1/b.c Ex2/main.c
EXECUTABLES = $(ENTRY_POINTS:%.c=$(BIN_DIR)/%)

# Create bin directory if not exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Default target
all: $(BIN_DIR) $(EXECUTABLES)

# Build each executable
$(BIN_DIR)/%: %.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Clean up build artifacts
clean:
	rm -rf $(BIN_DIR)

# Rebuild everything
rebuild: clean all

.PHONY: all clean rebuild