#!/bin/sh

CC = gcc
CFLAGS = -g -Wall

TARGET = ccwc

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $< -o $(TARGET)


.PHONY: all memcheck clean

memcheck:
	valgrind -s --leak-check=full ./ccwc test.txt 2>err.txt

clean:
	-rm -f a.out
	-rm -f ccwc