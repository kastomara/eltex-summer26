CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc
AR = ar
ARFLAGS = rcs

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity
LIB_DIR = lib
BIN_DIR = bin

SRCS = $(SRC_DIR)/calc.c
MAIN_SRC = main.c
TEST_SRC = $(TEST_DIR)/test_calc.c
UNITY_SRC = $(UNITY_DIR)/unity.c
HEADERS = $(SRC_DIR)/calc.h $(SRC_DIR)/list.h
LIB_SRCS = $(SRC_DIR)/list.c $(SRC_DIR)/calc.c

TARGET = $(BIN_DIR)/app
TEST_TARGET = $(BIN_DIR)/test_run
STATIC_LIB = $(LIB_DIR)/liblist.a

APP_OBJS = $(BIN_DIR)/main.o $(BIN_DIR)/calc.o
LIB_OBJS = $(BIN_DIR)/list.o $(BIN_DIR)/calc.o
TEST_OBJS = $(BIN_DIR)/test_calc.o $(BIN_DIR)/calc.o $(BIN_DIR)/unity.o

all: directories $(STATIC_LIB) $(TARGET)

directories:
	mkdir -p $(BIN_DIR) $(LIB_DIR)

$(STATIC_LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(TARGET): $(APP_OBJS) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJS) -L$(LIB_DIR) -llist

$(TEST_TARGET): $(TEST_OBJS) $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) -L$(LIB_DIR) -llist

$(BIN_DIR)/main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/calc.o: $(SRC_DIR)/calc.c $(SRC_DIR)/calc.h $(SRC_DIR)/list.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/list.o: $(SRC_DIR)/list.c $(SRC_DIR)/list.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/test_calc.o: $(TEST_DIR)/test_calc.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/unity.o: $(UNITY_DIR)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR)/*.o $(BIN_DIR)/*.a
	rm -f $(TARGET) $(TEST_TARGET) $(STATIC_LIB)
	rm -rf $(BIN_DIR) $(LIB_DIR)

rebuild: clean all

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean rebuild run debug valgrind install test directories