CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

TARGET = app
TEST_TARGET = test_run

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity

SRCS = $(SRC_DIR)/calc.c
MAIN_SRC = $(SRC_DIR)/main.c
TEST_SRC = $(TEST_DIR)/test_calc.c
UNITY_SRC = $(UNITY_DIR)/unity.c
HEADERS = $(SRC_DIR)/calc.h

APP_OBJS = $(SRC_DIR)/calc.o $(SRC_DIR)/main.o

all: $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(APP_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(APP_OBJS)

clean:
	rm -f $(APP_OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) 192.168.1.0 /24 10

.PHONY: all clean run test