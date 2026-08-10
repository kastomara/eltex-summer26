CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iunity -Isrc -fPIC
AR = ar
ARFLAGS = rcs

SRC_DIR = src
TEST_DIR = tests
UNITY_DIR = unity
LIB_DIR = lib
BIN_DIR = bin

SRCS = $(SRC_DIR)/pb_queue.c
MAIN_SRC = main.c
TEST_SRC = $(TEST_DIR)/test_pb_queue.c
UNITY_SRC = $(UNITY_DIR)/unity.c
HEADERS = $(SRC_DIR)/pb_queue.h $(SRC_DIR)/list.h
LIB_SRCS = $(SRC_DIR)/list.c $(SRC_DIR)/pb_queue.c

TARGET = $(BIN_DIR)/app
TEST_TARGET = $(BIN_DIR)/test_run
DYNAMIC_LIB = $(LIB_DIR)/liblist.dylib

APP_OBJS = $(BIN_DIR)/main.o $(BIN_DIR)/pb_queue.o
LIB_OBJS = $(BIN_DIR)/list.o $(BIN_DIR)/pb_queue.o
TEST_OBJS = $(BIN_DIR)/test_pb_queue.o $(BIN_DIR)/pb_queue.o $(BIN_DIR)/unity.o

all: directories $(DYNAMIC_LIB) $(TARGET)

directories:
	mkdir -p $(BIN_DIR) $(LIB_DIR)

$(DYNAMIC_LIB): $(LIB_OBJS)
	$(CC) -dynamiclib -install_name @rpath/liblist.dylib -o $@ $^

$(TARGET): $(APP_OBJS) $(DYNAMIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJS) -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR) -llist

$(TEST_TARGET): $(TEST_OBJS) $(DYNAMIC_LIB)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) -L$(LIB_DIR) -Wl,-rpath,$(LIB_DIR) -llist

$(BIN_DIR)/main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/pb_queue.o: $(SRC_DIR)/pb_queue.c $(SRC_DIR)/pb_queue.h $(SRC_DIR)/list.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/list.o: $(SRC_DIR)/list.c $(SRC_DIR)/list.h
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/test_pb_queue.o: $(TEST_DIR)/test_pb_queue.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/unity.o: $(UNITY_DIR)/unity.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(BIN_DIR)/*.o $(BIN_DIR)/*.dylib
	rm -f $(TARGET) $(TEST_TARGET) $(DYNAMIC_LIB)
	rm -rf $(BIN_DIR) $(LIB_DIR)

rebuild: clean all

test: $(TEST_TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ./$(TEST_TARGET)

run: $(TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH ./$(TARGET)

.PHONY: all clean rebuild run debug valgrind install test directories