CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

TARGET = app
TEST_TARGET = test_run

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity
BIN_DIR = bin

SRC_FILES = $(SRC_DIR)/calc.c
TEST_FILES = $(TEST_DIR)/test_calc.c $(UNITY_DIR)/unity.c

all: $(TARGET)

$(TARGET): $(SRC_FILES) $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_FILES)
$(TEST_TARGET): $(SRC_FILES) $(TEST_FILES)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(SRC_FILES) $(TEST_FILES)
test: $(TEST_TARGET)
	./$(TEST_TARGET)
run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(UNITY_DIR)/*.o
	rm -f $(TARGET) $(TEST_TARGET)
.PHONY: all clean rebuild run debug install test coverage