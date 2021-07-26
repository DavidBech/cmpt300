# Target Names
.PHONY: all, clean, run, valgrind

# Makefile Variables
PROGNAME := UnixLs
SET_TEST := set_test
SRC_DIR := src
OBJ_DIR := obj
INC_DIR := headers
local_CFLAGS := $(CFLAGS) -Wall -Werror -g -iquote $(INC_DIR)
local_LFLAGS := $(LFLAGS)

# Default Targets and Clean Targets
all: dirs $(PROGNAME) 

dirs: $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p obj

clean: 
	rm -f $(PROGNAME)
	rm -f $(SET_TEST)
	rm -rf $(OBJ_DIR)

# gcc Targets
$(SET_TEST): dirs $(OBJ_DIR)/set.o $(OBJ_DIR)/set_testing_main.o
	gcc $(local_LFLAGS) -o $(SET_TEST) $(OBJ_DIR)/set.o $(OBJ_DIR)/set_testing_main.o

$(OBJ_DIR)/set_testing_main.o: $(SRC_DIR)/set_testing_main.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/set_testing_main.c -o $(OBJ_DIR)/set_testing_main.o

$(PROGNAME): $(OBJ_DIR)/UnixLs.o $(OBJ_DIR)/set.o
	gcc $(local_LFLAGS) -o $(PROGNAME) $(OBJ_DIR)/UnixLs.o $(OBJ_DIR)/set.o

$(OBJ_DIR)/UnixLs.o: $(SRC_DIR)/UnixLs.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/UnixLs.c -o $(OBJ_DIR)/UnixLs.o

$(OBJ_DIR)/set.o: $(SRC_DIR)/set.c $(INC_DIR)/set.h
	gcc $(local_CFLAGS) -c $(SRC_DIR)/set.c -o $(OBJ_DIR)/set.o

