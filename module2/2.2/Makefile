CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity

SRCS = $(SRC_DIR)/calc.c
MAIN_SRC = $(SRC_DIR)/main.c
TEST_SRC = $(TEST_DIR)/test_calc.c
UNITY_SRC = $(UNITY_DIR)/unity.c
HEADERS = $(SRC_DIR)/calc.h

$(shell mkdir -p bin)

TARGET = bin/app
TEST_TARGET = bin/test_run

APP_OBJS = $(SRC_DIR)/calc.o $(SRC_DIR)/main.o

TEST_OBJS = $(SRC_DIR)/calc.o $(TEST_DIR)/test_calc.o $(UNITY_DIR)/unity.o

all: $(TARGET)

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

rebuild: clean all

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: clean all

valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

install: $(TARGET)
	chmod +x $(TARGET)

.PHONY: all clean rebuild run debug valgrind install test