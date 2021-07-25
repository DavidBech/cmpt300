#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>

typedef struct unix_ls_arg_s unix_ls_arg;
struct unix_ls_arg_s{
    bool inode;
    bool long_list;
    bool recursive;
};

// Called with a given file/directory to print as well as additional info about what print paramaters
void print_file(struct dirent* file_to_print, unix_ls_arg* paramaters);

// Call print_file on each file in the given dir name
void print_dir(char* path_to_dir, unix_ls_arg* paramaters);

int main(int argc, char** argv){
    unix_ls_arg ls_args;
    memset(&ls_args, '\0', sizeof(ls_args));
    int dir_index = 0;

    // Set the input arguments parameters
    for(int i=1; i<argc; ++i){
        if(argv[i][0] == '-'){
            for(int j=1; j<strlen(argv[i]); ++j){
                switch(argv[i][j]){
                    case('i'):
                        ls_args.inode = true;
                        break;
                    case('l'):
                        ls_args.long_list = true;
                        break;
                    case('R'):
                        ls_args.recursive = true;
                        break;
                    default:
                        fprintf(stderr, "UnixLs invalid option -- '%c'", argv[i][j]);
                        return EXIT_FAILURE;
                }
            }
        } else {
            // Files and directories come after command line inputs
            dir_index = i;
            break;
        }
    }

    // TODO remove temparary info
    printf("Inode: %s\n", ls_args.inode ? "True":"False");
    printf("Long Output: %s\n", ls_args.long_list ? "True":"False");
    printf("Recursive %s\n", ls_args.recursive ? "True":"False");
    printf("Index: %u\n", dir_index);
    printf("\n");

    // TODO Recursion
    if(dir_index == 0){
        print_dir(".", &ls_args);
    } else {
        // TODO
    }

    return EXIT_SUCCESS;
}

void print_dir(char* path_to_dir, unix_ls_arg* params){
    assert(path_to_dir != NULL);
    DIR* directory = opendir(path_to_dir);
    if(!directory){
        fprintf(stderr, "UnixLs: cannot access '%s': No such file or directory\n", path_to_dir);
        return;
    }
    struct dirent* dir_file = readdir(directory);
    do{
        print_file(dir_file, params);
        dir_file = readdir(directory);
    } while (dir_file != NULL);
    if(errno){
        fprintf(stderr, "Erno Set Error\n");
        // TODO readdir failed
    }
    
    if(closedir(directory)){
        // TODO close dir error
        fprintf(stderr, "Close Dir Error\n");
        return;
    }
    printf("\n");
}


void print_file(struct dirent* file_to_print, unix_ls_arg* params){
    // TODO PARAMS
    // TODO FORMAT
    if(file_to_print->d_name[0] == '.'){
        return;
    }
    printf("%s  ", file_to_print->d_name);
}

