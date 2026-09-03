CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Isrc

SRC_DIR = src
BIN_DIR = bin

TARGET_S = $(BIN_DIR)/server
TARGET_C = $(BIN_DIR)/client

all: directories $(TARGET_S) $(TARGET_C)

directories:
	mkdir -p $(BIN_DIR)

$(TARGET_S): $(SRC_DIR)/raw_client.c $(SRC_DIR)/raw.h
	$(CC) $(CFLAGS) -o $(TARGET_S) $(SRC_DIR)/tcpserver.c

$(TARGET_C): $(SRC_DIR)/raw_server.c $(SRC_DIR)/raw.h
	$(CC) $(CFLAGS) -o $(TARGET_C) $(SRC_DIR)/tcpclient.c

clean:
	rm -f $(TARGET_S) $(TARGET_C)
	rm -rf $(BIN_DIR)

.PHONY: all clean run 