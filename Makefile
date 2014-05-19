CC=clang
CFLAGS=-Wall -lm -Wextra -std=c99 -g -lSDL

all:
	$(CC) $(CFLAGS) kohonen.c main.c

