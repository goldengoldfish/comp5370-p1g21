.PHONY: clean
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g $(shell pkg-config --cflags libssl)
LDLIBS = -lcrypto
TARGET = da

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
HEADERS = $(wildcard *.h)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(OBJS): $(HEADERS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.DEFAULT_GOAL := $(TARGET)
