.PHONY: all, clean, run, valgrind
PROGNAME := a.out

RM := rm -f
COMP_FLAGS := -Wall -Werror -g

SRC_FILES = $(wildcard ./*.c)

INC_FILES = $(wildcard ./*.h)

OBJ_FILES = $(SRC_FILES:%.c=%.o)

all: $(PROGNAME)

$(PROGNAME): $(OBJ_FILES)
	gcc -o $@ $^

$(OBJ_FILES): %.o : %.c
	gcc $(COMP_FLAGS) -c $< -o $@

clean: 
	$(RM) $(PROGNAME)
	$(RM) *.o

run: all
	./$(PROGNAME)

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(PROGNAME)
