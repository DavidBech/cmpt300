#include <stdbool.h>
#include <sys/stat.h>

#ifndef __UNIX_LS_H
#define __UNIX_LS_H

typedef struct unix_ls_arg_s unix_ls_arg;
struct unix_ls_arg_s{
    bool inode;
    bool long_list;
    bool recursive;
};

// Prints the given file
//  if the name is a dir prints all files in the dir
void print_file(char* file_name, unix_ls_arg* paramaters, bool header);

// Prints the input dirent
void print_dirent(char* dir_name, struct dirent* file_to_print, unix_ls_arg* paramaters);

// Prints the input stat
void print_stat(char* file_name, struct stat* file_to_print, unix_ls_arg* paramaters);

// Returns date string in the string buffer for long list printout
void get_date_string(struct timespec time_stamp, char* string_buffer);

// Returns mode string in string buffer for long list printout
void get_mode_string(mode_t mode, char* string_buffer);

// Returns true if given file is a dir
bool is_dir(char* file_name, char* path_name);

#endif