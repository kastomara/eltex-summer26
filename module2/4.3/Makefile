CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity
BIN_DIR = bin

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/pb_tree.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRC_DIR)/pb_tree.h

TEST_SRCS = $(TEST_DIR)/test_pb_tree.c $(SRC_DIR)/pb_tree.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_HEADERS = $(SRC_DIR)/pb_tree.h $(UNITY_DIR)/unity.h
UNITY_SRC = $(UNITY_DIR)/unity.c
UNITY_OBJ = $(UNITY_DIR)/unity.o

TARGET = $(BIN_DIR)/app
TEST_TARGET = $(BIN_DIR)/test_run

all: directories $(TARGET)

directories:
	mkdir -p $(BIN_DIR) 
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(UNITY_OBJ)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(UNITY_OBJ)

$(UNITY_OBJ): $(UNITY_SRC)
	$(CC) $(CFLAGS) -c $(UNITY_SRC) -o $(UNITY_OBJ)

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET) $(UNITY_OBJ)
	rm -rf $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run 