# Target Names
.PHONY: all, clean, run, valgrind

# Makefile Variables
PROGNAME := kernel_sim
SRC_DIR := src
INC_DIR := headers
OBJ_DIR := obj
local_CFLAGS := $(CFLAGS) -Wall -Werror -g -iquote $(INC_DIR)
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
$(PROGNAME): $(OBJ_DIR)/main.o $(OBJ_DIR)/list.o $(OBJ_DIR)/process_cb.o $(OBJ_DIR)/kernel_sim.o $(OBJ_DIR)/executioner.o $(OBJ_DIR)/queue_manager.o $(OBJ_DIR)/sem.o
	gcc $(local_LFLAGS) -o $(PROGNAME) $(OBJ_DIR)/main.o $(OBJ_DIR)/list.o $(OBJ_DIR)/kernel_sim.o $(OBJ_DIR)/executioner.o $(OBJ_DIR)/queue_manager.o $(OBJ_DIR)/process_cb.o $(OBJ_DIR)/sem.o

$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/main.c -o $(OBJ_DIR)/main.o

$(OBJ_DIR)/list.o: $(SRC_DIR)/list.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/list.c -o $(OBJ_DIR)/list.o 

$(OBJ_DIR)/process_cb.o: $(SRC_DIR)/process_cb.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/process_cb.c -o $(OBJ_DIR)/process_cb.o 

$(OBJ_DIR)/kernel_sim.o: $(SRC_DIR)/kernel_sim.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/kernel_sim.c -o $(OBJ_DIR)/kernel_sim.o 

$(OBJ_DIR)/executioner.o: $(SRC_DIR)/executioner.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/executioner.c -o $(OBJ_DIR)/executioner.o 

$(OBJ_DIR)/queue_manager.o: $(SRC_DIR)/queue_manager.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/queue_manager.c -o $(OBJ_DIR)/queue_manager.o 

$(OBJ_DIR)/sem.o: $(SRC_DIR)/sem.c
	gcc $(local_CFLAGS) -c $(SRC_DIR)/sem.c -o $(OBJ_DIR)/sem.o 





