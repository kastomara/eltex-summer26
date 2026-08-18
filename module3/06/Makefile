CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

SRC_DIR = src
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/updsock.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRC_DIR)/updsock.h

TARGET = $(BIN_DIR)/app

all: directories $(TARGET)

directories:
	mkdir -p $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(BIN_DIR)
	-ipcrm -a 2>/dev/null || true

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run 