CC = gcc
CFLAGS = -I../ -g -Wall -Wextra -std=c89 -pedantic

SRCS = $(wildcard *.c)

PROGS = $(patsubst %.c,%,$(SRCS))

all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $< ../iconf.c

