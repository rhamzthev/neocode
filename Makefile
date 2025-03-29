CC = gcc

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

CFLAGS = -Wall -Wextra -I./$(INC_DIR)
LDFLAGS = 

# List of source files (updated to include terminal.c)
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TARGET = $(BIN_DIR)/ncode

# Default target
all: dirs $(TARGET)

# Create necessary directories
dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR)

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the editor
run: all
	./$(TARGET) $(ARGS)

.PHONY: all clean run dirs