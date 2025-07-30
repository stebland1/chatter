CC = gcc
CFLAGS = -Wall -Wextra -I./include
SERVER_SRC = $(wildcard src/server/*.c)
CLIENT_SRC = $(wildcard src/client/*.c)
BIN_DIR = build
OBJ_SERVER_DIR = $(BIN_DIR)/server/obj
OBJ_CLIENT_DIR = $(BIN_DIR)/client/obj
OBJ_SERVER = $(patsubst src/server/%.c, $(OBJ_SERVER_DIR)/%.o, $(SERVER_SRC))
OBJ_CLIENT = $(patsubst src/client/%.c, $(OBJ_CLIENT_DIR)/%.o, $(CLIENT_SRC))
SERVER_TARGET = $(BIN_DIR)/bin/server
CLIENT_TARGET = $(BIN_DIR)/bin/client

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(OBJ_SERVER)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_SERVER_DIR)/%.o: src/server/%.c
	@mkdir -p $(OBJ_SERVER_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(CLIENT_TARGET): $(OBJ_CLIENT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_CLIENT_DIR)/%.o: src/client/%.c
	@mkdir -p $(OBJ_CLIENT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)
