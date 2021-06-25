# This is the Makefile
CC = gcc
FLAG = -Wall -Werror -g

all: build

build:
	$(CC) $(FLAG) demo_condition.c general.c printer.c signaller.c -lpthread -o main

run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main
 
clean:
	rm -rf main *.o *.exe *.out