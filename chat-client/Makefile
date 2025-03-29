# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinc

# Directories
SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = 	$(SRC_DIR)/main.c \
		$(SRC_DIR)/messageFunctions.c \
		$(SRC_DIR)/programFunctions.c \
		$(SRC_DIR)/UIFunctions.c

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executable name
TARGET = chat-client

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OBJ_DIR) $(BIN_DIR)
