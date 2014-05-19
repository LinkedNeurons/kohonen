CC=clang
CFLAGS=-Wall -lm -Wextra -std=c99 -g -lSDL -lSDL_draw

all:
	$(CC) $(CFLAGS) kohonen.c main.c

