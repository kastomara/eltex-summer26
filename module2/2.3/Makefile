CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc
LDFLAGS = -lm

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity
BIN_DIR = bin

SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/calc.c $(SRC_DIR)/parser.c
TEST_FILES = $(TEST_DIR)/test_calc.c $(UNITY_DIR)/unity.c

OBJS = $(SRC_FILES:.c=.o)
TEST_OBJS = $(SRC_FILES:.c=.o) $(TEST_FILES:.c=.o)
HEADERS = $(SRC_DIR)/calc.h

TARGET = $(BIN_DIR)/runner
TEST_TARGET = $(BIN_DIR)/test_runner

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) -lm
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(UNITY_DIR)/*.o
	rm -f $(TARGET) $(TEST_TARGET)
run: $(TARGET)
	./$(TARGET)
test: $(TEST_TARGET)
	./$(TEST_TARGET)
rebuild: clean run

.PHONY: all clean run rebuild