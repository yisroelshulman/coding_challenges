#shell
SHELL = /bin/sh

CC = gcc
CFLAGS = -g -Wall

all: ccwc.c
	$(CC) $(CFLAGS) $< -o ccwc


.PHONY: all memcheck clean

memcheck:
	valgrind -s --leak-check=full ./ccwc test.txt 2>err.txt

clean:
	-rm -f a.out
	-rm -f ccwc