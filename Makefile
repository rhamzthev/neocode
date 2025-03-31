CC = gcc

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Update include paths to look in subdirectories
CFLAGS = -Wall -Wextra -I./$(INC_DIR) -I./$(INC_DIR)/core -I./$(INC_DIR)/ui -I./$(INC_DIR)/io -I./$(INC_DIR)/utils
LDFLAGS = 

# Find all source files in the new directory structure
SRCS = $(wildcard $(SRC_DIR)/*.c) \
	   $(wildcard $(SRC_DIR)/core/*.c) \
	   $(wildcard $(SRC_DIR)/ui/*.c) \
	   $(wildcard $(SRC_DIR)/io/*.c) \
	   $(wildcard $(SRC_DIR)/utils/*.c)

# Transform source paths to corresponding object paths
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TARGET = $(BIN_DIR)/ncode

# Default target
all: dirs $(TARGET)

# Create necessary directories including subdirectories for objects
dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR) $(OBJ_DIR)/core $(OBJ_DIR)/ui $(OBJ_DIR)/io $(OBJ_DIR)/utils

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files, preserving directory structure
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the editor
run: all
	./$(TARGET) $(ARGS)

# Debug info - print discovered sources and objects
debug:
	@echo "Sources:"
	@echo $(SRCS)
	@echo "Objects:"
	@echo $(OBJS)

.PHONY: all clean run dirs debug