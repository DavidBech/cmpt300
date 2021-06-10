.PHONY: all, clean, run, valgrind, dirs
PROGNAME := a.out

INC_P = 42069
OUT_M = "Out_Machine_Name"
OUT_P = 69420

RM := rm -f
SRC_DIR := src
INC_DIR := headers
OBJ_DIR := obj
BIN_DIR := .

CFLAGS := $(CFLAGS) -Wall -Werror -g -iquote $(INC_DIR)
LFLAGS := $(LFLAGS) -pthread -lpthread

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
INC_FILES = $(wildcard $(INC_DIR)/*.h)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(BIN_DIR)/$(PROGNAME) 

$(BIN_DIR)/$(PROGNAME): $(OBJ_FILES)
	gcc $(LFLAGS) -o $@ $^
 
$(OBJ_FILES): $(OBJ_DIR)/%.o :  $(SRC_DIR)/%.c
	gcc $(CFLAGS) -c $< -o $@

clean: 
	$(RM) $(BIN_DIR)/$(PROGNAME)
	$(RM) $(OBJ_DIR)/*.o
 
run: all
	./$(BIN_DIR)/$(PROGNAME) $(INC_P) $(OUT_M) $(OUT_P)
 
valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(BIN_DIR)/$(PROGNAME) $(INC_P) $(OUT_M) $(OUT_P)
