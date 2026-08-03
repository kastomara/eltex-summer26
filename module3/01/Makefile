CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

SRC_DIR = src
BIN_DIR = bin
UNITY_DIR = unity

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/copy.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRC_DIR)/copy.h

TEST_SRCS = $(TEST_DIR)/test_calc.c $(SRC_DIR)/copy.c $(UNITY_DIR)/unity.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_HEADERS = $(SRC_DIR)/calc.h $(UNITY_DIR)/unity.h

TARGET = $(BIN_DIR)/app
TEST_TARGET = $(BIN_DIR)/tester

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
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_OBJS)

$(UNITY_OBJ): $(UNITY_SRC)
	$(CC) $(CFLAGS) -c $(UNITY_SRC) 

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)
	rm -rf $(BIN_DIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run 