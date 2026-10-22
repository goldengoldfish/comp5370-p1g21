.PHONY: all clean

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g
LDLIBS = -lcrypto
TARGET = da

SRCS = src/main.c
OBJS = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h)

.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

src/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
