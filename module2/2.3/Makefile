CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity

SRCS = $(SRC_DIR)/calc.c $(SRC_DIR)/parser.c
MAIN_SRC = $(SRC_DIR)/main.c
TEST_SRC = $(TEST_DIR)/test_calc.c
UNITY_SRC = $(UNITY_DIR)/unity.c
HEADERS = $(SRC_DIR)/calc.h

$(shell mkdir -p bin)

TARGET = bin/app
TEST_TARGET = bin/test_run

APP_OBJS = $(SRC_DIR)/calc.o $(SRC_DIR)/main.o $(SRC_DIR)/parser.o

TEST_OBJS = $(SRC_DIR)/calc.o $(TEST_DIR)/test_calc.o $(UNITY_DIR)/unity.o $(SRC_DIR)/parser.o

all: directories $(TARGET)

directories:
	mkdir -p $(BIN_DIR) 

$(TARGET): $(APP_OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(APP_OBJS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(UNITY_DIR)/%.o: $(UNITY_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(UNITY_DIR)/*.o $(TARGET) $(TEST_TARGET)
	rm -rf $(BIN_DIR)

rebuild: clean all

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run debug valgrind install test