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

// Returns hash of string
unsigned long string_hash(char* str);

// Compares stirng
bool compare_string(char* str0, char* str1);

// Called with a given file/directory to print as well as additional info about what print paramaters
void print_file(struct dirent* file_to_print, unix_ls_arg* paramaters);

// Call print_file on each file in the given dir name
void print_dir(char* name, unix_ls_arg* paramaters, bool header);

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

    bool header = dir_index != 0 && argc - dir_index > 1;
    // TODO Current implementation will fail if input is file not directory
    // TODO Recursion
    if(dir_index == 0){
        print_dir(".", &ls_args, header);
    } else { 
        for(int i=dir_index; i<argc; ++i){
            print_dir(argv[i], &ls_args, header);
            if(i != argc -1){
                printf("\n");
            }
        }
    }

    return EXIT_SUCCESS;
}

void print_dir(char* name, unix_ls_arg* params, bool header){
    assert(name != NULL);
    DIR* directory = opendir(name);
    if(!directory){
        // TODO handle name is file name not directory name
        errno = 0;
        fprintf(stderr, "UnixLs: cannot access '%s': No such file or directory\n", name);
        return;
    }
    if(header){
        printf("%s:\n", name);
    }

    struct dirent* dir_file = readdir(directory);
    do{
        
        print_file(dir_file, params);
        dir_file = readdir(directory);
    } while (dir_file != NULL);
    if(errno){
        fprintf(stderr, "Erno Set Error\n");
        errno = 0;
        return;
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

// This is not my hashing function 
//  Source: https://stackoverflow.com/questions/7666509/hash-function-for-string
unsigned long string_hash(char* str){
    unsigned long hash = 5281;
    int c = *str;
    while(c){
        hash = ((hash << 5) + hash) + c;
        c = *(++str);
    }
    return hash;
}

bool compare_string(char* str0, char* str1){
    return strcmp(str0, str1) == 0;
}
