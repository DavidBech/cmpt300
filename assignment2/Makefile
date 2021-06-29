.PHONY: all, clean, run, valgrind
PROGNAME := s-talk

PORT1 = 12345
MACHINE1 = "128.0.0.1"
PORT2 = 69420
MACHINE2 = "128.0.0.1"

RM := rm -f
SRC_DIR := src
INC_DIR := headers

local_CFLAGS := $(CFLAGS) -Wall -Werror -g -iquote $(INC_DIR)
local_LFLAGS := $(LFLAGS) -pthread -lpthread

all: dirs $(PROGNAME) 

dirs: obj log

obj:
	mkdir -p obj

log:
	mkdir -p log

$(PROGNAME): obj/main.o obj/list.o obj/stalk.o obj/udp_rx.o obj/udp_tx.o obj/user_display.o obj/user_reader.o
	gcc $(local_LFLAGS) -o $(PROGNAME) obj/main.o obj/list.o obj/stalk.o obj/udp_rx.o obj/udp_tx.o obj/user_display.o obj/user_reader.o

obj/main.o: src/main.c
	gcc $(local_CFLAGS) -c src/main.c -o obj/main.o

obj/list.o: src/list.c
	gcc $(local_CFLAGS) -c src/list.c -o obj/list.o 

obj/stalk.o: src/stalk.c
	gcc $(local_CFLAGS) -c src/stalk.c -o obj/stalk.o 

obj/udp_rx.o: src/udp_rx.c
	gcc $(local_CFLAGS) -c src/udp_rx.c -o obj/udp_rx.o 

obj/udp_tx.o: src/udp_tx.c
	gcc $(local_CFLAGS) -c src/udp_tx.c -o obj/udp_tx.o 

obj/user_display.o: src/user_display.c
	gcc $(local_CFLAGS) -c src/user_display.c -o obj/user_display.o 

obj/user_reader.o: src/user_reader.c
	gcc $(local_CFLAGS) -c src/user_reader.c -o obj/user_reader.o 

clean: 
	$(RM) $(PROGNAME)
	$(RM) log/*
	$(RM) obj/*
 
run: all
	./$(PROGNAME) $(PORT1) $(MACHINE2) $(PORT2)

run1: all
	./$(PROGNAME) $(PORT2) $(MACHINE1) $(PORT1)
 
valgrind: all
	valgrind ./$(PROGNAME) $(PORT2) $(MACHINE1) $(PORT1)
