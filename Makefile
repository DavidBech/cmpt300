# Target Names
.PHONY: all, clean, run, valgrind

# Makefile Variables
PROGNAME := kernel_sim
SRC_DIR := src
INC_DIR := headers
OBJ_DIR := obj
local_CFLAGS := $(CFLAGS) -Wall -Werror -g -iquote $(INC_DIR)
local_LFLAGS := $(LFLAGS) -pthread -lpthread

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
$(PROGNAME): $(OBJ_DIR)/main.o $(OBJ_DIR)/list.o
	gcc $(local_LFLAGS) -o $(PROGNAME) $(OBJ_DIR)/main.o $(OBJ_DIR)/list.o

obj/main.o: $(SRC_DIR)/main.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

obj/list.o: $(SRC_DIR)/list.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o 
