CC = gcc
CFLAGS = -fPIC -Wall -O2 -I. -arch x86_64 -arch arm64
LDFLAGS = -shared -flat_namespace -undefined suppress
LIBS = -lm -ldl

SRC_DIR = src
LIB_DIR = lib
BIN_DIR = bin

TARGET = $(BIN_DIR)/app
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/loader.c $(SRC_DIR)/parser.c
OBJS = $(SRCS:.c=.o)

LIB_SRCS = $(LIB_DIR)/add.c $(LIB_DIR)/subtract.c $(LIB_DIR)/multiply.c $(LIB_DIR)/divide.c $(LIB_DIR)/max.c $(LIB_DIR)/power.c
LIB_EXT = dylib
LIB_TARGET = $(LIB_SRCS:.c=.$(LIB_EXT))

all: directories libs $(TARGET) 

directories:
	mkdir -p $(BIN_DIR) 

libs: $(LIB_TARGET)

%.$(LIB_EXT): %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<
	@echo "Собрана: $@"
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS) -arch x86_64 -arch arm64

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIB_TARGET) $(OBJS) $(TARGET)
	rm -rf $(BIN_DIR)
	@echo "Очищено"

.PHONY: all clean run libs