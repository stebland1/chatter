CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC = $(wildcard src/*.c)
OBJ_DIR = build/obj
BIN_DIR = build
OBJ = $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRC))
SERVER_TARGET = $(BIN_DIR)/server

all: $(SERVER_TARGET)

$(SERVER_TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR)
