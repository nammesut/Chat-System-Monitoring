# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -g -Iinc
LDFLAGS = -pthread -lrt

# Directories
SRC_DIR_DIVLIB     = divlib
SRC_DIR_SYSTEM     = system

SERVER_DIR  = $(SRC_DIR_DIVLIB)/server/src
CLIENT_DIR  = $(SRC_DIR_DIVLIB)/client
DAEMON_DIR  = $(SRC_DIR_SYSTEM)/sys/src
IPC_DIR     = $(SRC_DIR_SYSTEM)/ipc
BIN_DIR     = bin

# Source files
SERVER_SRCS = $(wildcard $(SERVER_DIR)/*.c) $(wildcard $(IPC_DIR)/*.c)
CLIENT_SRCS = $(wildcard $(CLIENT_DIR)/*.c)
DAEMON_SRCS = $(wildcard $(DAEMON_DIR)/*.c) $(wildcard $(IPC_DIR)/*.c)

# Object files
SERVER_OBJS = $(SERVER_SRCS:.c=.o)
CLIENT_OBJS = $(CLIENT_SRCS:.c=.o)
DAEMON_OBJS = $(DAEMON_SRCS:.c=.o)

# Targets
SERVER_TARGET = $(BIN_DIR)/server_app
CLIENT_TARGET = $(BIN_DIR)/client_app
DAEMON_TARGET = $(BIN_DIR)/monitor_daemon

.PHONY: all clean

all: $(BIN_DIR) $(SERVER_TARGET) $(CLIENT_TARGET) $(DAEMON_TARGET)

# Create bin dir if not exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Build server app
$(SERVER_TARGET): $(SERVER_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build client app
$(CLIENT_TARGET): $(CLIENT_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build monitor daemon
$(DAEMON_TARGET): $(DAEMON_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile rule (generic for all .c in project)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean all
clean:
	rm -rf $(SERVER_DIR)/*.o $(CLIENT_DIR)/*.o $(DAEMON_DIR)/*.o $(IPC_DIR)/*.o $(BIN_DIR)
