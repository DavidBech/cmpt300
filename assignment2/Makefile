.PHONY: all, clean, run, valgrind
PROGNAME := s-talk

PORT1 = 12345
MACHINE1 = "TOOD_MACHINE1"
PORT2 = 69420
MACHINE2 = "TOOD_MACHINE2"

RM := rm -f
SRC_DIR := src
INC_DIR := headers

local_CFLAGS := $(CFLAGS) -Wall -Werror -g -iquote $(INC_DIR)
local_LFLAGS := $(LFLAGS) -pthread -lpthread

all: $(PROGNAME) 

$(PROGNAME): src/main.c src/list.c src/stalk.c src/udp_rx.c src/udp_tx.c src/user_display.c src/user_reader.c
	gcc $(local_LFLAGS) $(local_CFLAGS) -o $(PROGNAME) src/main.c src/list.c src/stalk.c src/udp_rx.c src/udp_tx.c src/user_display.c src/user_reader.c
 
clean: 
	$(RM) $(PROGNAME)
	$(RM) log/*
 
run: all
	./$(PROGNAME) $(PORT1) $(MACHINE2) $(PORT2)

run1: all
	./$(PROGNAME) $(PORT2) $(MACHINE1) $(PORT1)
 
valgrind: all
	valgrind ./$(BIN_DIR)/$(PROGNAME) $(PORT2) $(MACHINE1) $(PORT1)
