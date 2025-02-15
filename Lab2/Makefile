# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS = 

# Directories
BIN_DIR = ./bin

# Ensure bin directory exists before compiling
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build a specific file
build: $(BIN_DIR)/$(file)

$(BIN_DIR)/%.c: %.c | $(BIN_DIR)
	@$(CC) $(CFLAGS) -o $(BIN_DIR)/$(basename $(notdir $<)) $< $(LDFLAGS)

# Run a specific file (ensures it's built first)
run: build
	@$(BIN_DIR)/$(basename $(notdir $(file)))

# Clean all compiled executables
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BIN_DIR)

.PHONY: build run clean