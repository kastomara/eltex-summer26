CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity
BIN_DIR = bin

<<<<<<< HEAD
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/ipgen.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRC_DIR)/ipgen.h

TEST_SRCS = $(TEST_DIR)/test_ipgen.c $(SRC_DIR)/ipgen.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_HEADERS = $(SRC_DIR)/ipgen.h $(UNITY_DIR)/unity.h
=======
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/calc.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(SRC_DIR)/calc.h

TEST_SRCS = $(TEST_DIR)/test_calc.c $(SRC_DIR)/calc.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_HEADERS = $(SRC_DIR)/calc.h $(UNITY_DIR)/unity.h
>>>>>>> a2f58e574dc0213e328b25463792a31c11913793
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