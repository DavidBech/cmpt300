#include <stdio.h>
#include <stdlib.h>

#include "list.h"

#define INVALID_INPUT 1

int main(int argc, char** argv){
    char *endptr;
    long in_port;
    long out_port;
    char *out_machine;

    in_port = strtol(argv[1], &endptr, 10);
    if(endptr == argv[1]){
        fprintf(stderr, "Invalid In Port input: %s", argv[1]);
        exit(INVALID_INPUT);
    }
    out_machine = argv[2];
    out_port = strtol(argv[3], &endptr, 10);
    if(endptr == argv[3]){
        fprintf(stderr, "Invalid In Port input: %s", argv[3]);
        exit(INVALID_INPUT);
    }

    printf("in_port: %ld \n", in_port);
    printf("out_machine: %s \n", out_machine);
    printf("out_port: %ld \n", out_port);

    return(0);
}

