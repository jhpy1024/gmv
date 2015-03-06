CC = gcc
CFLAGS = -std=c11 -Wall -Wextra

SRC_DIR = src
INC_DIR = include
BIN_DIR = bin
BIN_NAME = gmv
OBJ_DIR = $(BIN_DIR)/obj

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(SOURCES:.c=.o))

$(BIN_DIR)/$(BIN_NAME): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(BIN_DIR)/$(BIN_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I $(INC_DIR)/ -c $< -o $@

.PHONY: clean
clean:
	@rm -fv $(BIN_DIR)/$(BIN_NAME) $(OBJ_DIR)/*.o
