CC = gcc
# Add -Wno-stringop-truncation to suppress the strncpy warning
CFLAGS = -Wall -Wextra -O2 -Wno-stringop-truncation
LDFLAGS = -pthread
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
SRCS = $(SRC_DIR)/main.c
OBJS = $(OBJ_DIR)/main.o
TARGET = $(BIN_DIR)/chat-server

$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: clean