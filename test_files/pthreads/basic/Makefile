# This is the Makefile
CC = gcc
FLAG = -Wall -Werror -g

all: build

build:
	$(CC) $(FLAG) main.c general.c -lpthread -o main

run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main
 
clean:
	rm -rf main *.o *.exe *.out