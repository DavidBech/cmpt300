# Target Names
.PHONY: all, clean, run, valgrind

# Makefile Variables
PROGNAME := UnixLs
SRC_DIR := src
OBJ_DIR := obj
local_CFLAGS := $(CFLAGS) -Wall -Werror -g
local_LFLAGS := $(LFLAGS)

# Default Targets and Clean Targets
all: dirs $(PROGNAME) 

dirs: $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p obj

clean: 
	rm -f $(PROGNAME)
	rm -rf $(OBJ_DIR)

# Execution Targets
run: all
	./$(PROGNAME)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all ./$(PROGNAME)

# gcc Targets
$(PROGNAME): $(OBJ_DIR)/UnixLs.o
	gcc $(local_LFLAGS) -o $(PROGNAME) $(OBJ_DIR)/UnixLs.o

$(OBJ_DIR)/UnixLs.o: $(SRC_DIR)/UnixLs.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/UnixLs.c -o $(OBJ_DIR)/UnixLs.o


