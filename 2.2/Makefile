CC = gcc
CFLAGS = -Wall -Wextra -std=c11

TARGET = calculator

SRCS = main.c calc.c
OBJS = $(SRCS:.c=.o)

HEADERS = calc.h
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f $(OBJS) $(TARGET)
rebuild: clean all

run: $(TARGET)
	./$(TARGET)

debug: CFLAGS += -g -DDEBUG
debud: clean all
valgrind: $(TARGET)
	valgrind --leak-check=full ./$(TARGET)

install: $(TARGET)
	chmod +x $(TARGET)

.PHONY: all clean rebuild run debug valgrind install