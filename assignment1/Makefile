.PHONY: all, clean
PROGNAME := a.out

RM := rm -f

SRC_FILES = $(wildcard ./*.c)

INC_FILES = $(wildcard ./*.h)

OBJ_FILES = $(SRC_FILES:%.c=%.o)

all: $(PROGNAME)

$(PROGNAME): $(OBJ_FILES)
	gcc -o $@ $^

$(OBJ_FILES): %.o : %.c
	gcc -c $< -o $@

clean: 
	$(RM) $(PROGNAME)
	$(RM) *.o

run: all
	./$(PROGNAME)
