# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_XOPEN_SOURCE=500
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
	@chmod +x $(BIN_DIR)/$(basename $(notdir $(file))) # Ensure it's executable
	@$(BIN_DIR)/$(basename $(notdir $(file))) $(ARGS)

# Clean all compiled executables
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BIN_DIR)

.PHONY: build run clean