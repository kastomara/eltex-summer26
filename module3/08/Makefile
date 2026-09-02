CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_DEFAULT_SOURCE -D_GNU_SOURCE -Isrc

SRC_DIR = src
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/udp_sniffer.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRC_DIR)/udp_sniffer.h

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
.PHONY: all clean run 