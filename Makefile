# gcc -std=c99 -pedantic -Wall -Wextra main.c -o main
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
FILES=demos/main.c
INCLUDE=-Iinclude/CBSP
OUT=demos/demo

.PHONY: all build

build:
	$(CC) $(CFLAGS) $(INCLUDE) $(FILES) -o $(OUT)
