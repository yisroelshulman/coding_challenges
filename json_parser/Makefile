#shell
SHELL = /bin/sh

CC = gcc
CFLAGS = -g -Wall

all: json_parser.c
	$(CC) $(CFLAGS) $<


.PHONY:

memcheck:
	valgrind -s --leak-check=full ./a.out test.txt 2>err.txt

clean:
	-rm -f a.out