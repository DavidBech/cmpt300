.PHONY: all, clean, run, valgrind
PROGNAME := a.out

RM := rm -f
COMP_FLAGS := -Wall -Werror -g

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
INC_FILES = $(wildcard $(SRC_DIR)/*.h)

OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(BIN_DIR)/$(PROGNAME) 

$(BIN_DIR)/$(PROGNAME): $(OBJ_FILES)
	gcc -o $@ $^
 
$(OBJ_FILES): $(OBJ_DIR)/%.o :  $(SRC_DIR)/%.c
	gcc $(COMP_FLAGS) -c $< -o $@
 
clean: 
	$(RM) $(BIN_DIR)/$(PROGNAME)
	$(RM) $(OBJ_DIR)/*.o
 
run: all
	./$(PROGNAME)
 
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(PROGNAME)
