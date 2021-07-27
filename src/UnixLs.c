#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>

#include "UnixLs.h"
#include "set.h"
#include "fifo.h"

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

    // Print out info
    if(dir_index == 0){
        // Dir_index is zero when no file input args
        print_file(".", &ls_args, false);
    } else { 
        bool header = dir_index != 0 && argc - dir_index > 1;
        // Loop through the input file args printing them
        for(int i=dir_index; i<argc; ++i){
            print_file(argv[i], &ls_args, header);
        }
    }

    printf("\n");
    return EXIT_SUCCESS;
}

void print_file(char* name, unix_ls_arg* params, bool header){
    assert(name != NULL);
    DIR* directory = opendir(name);
    struct stat stat_buf;
    if(directory){
        // Input name is directory
        if(header){
            printf("%s:\n", name);
        }
        struct dirent* dir_file = readdir(directory);
        while (dir_file != NULL){   
            if(dir_file->d_name[0] != '.'){
                print_dirent(name, dir_file, params);    
            }
            if(params->recursive && is_dir(dir_file->d_name)){

            }

            dir_file = readdir(directory);
        }
        closedir(directory);
    } else if(!stat(name, &stat_buf)){
        // Input name is file
        print_stat(name, &stat_buf, params);
    } else {
        fprintf(stderr, "UnixLs: cannot access '%s': No such file or directory\n", name);
    }
    if(params->recursive){

    }
}

void print_dirent(char* dirname, struct dirent* file_to_print, unix_ls_arg* params){
    if(params->long_list){
        struct stat stat_buffer;
        char str_buffer[PATH_MAX];
        str_buffer[0] = '\0';
        strcat(str_buffer, dirname);
        strcat(str_buffer, "/");
        strcat(str_buffer, file_to_print->d_name);
        //TODO lstat vs stat
        if(lstat(str_buffer, &stat_buffer)){
            perror("lstat Failed\n");
            printf("%s\n", str_buffer);
            exit(EXIT_FAILURE);
        }
        print_stat(file_to_print->d_name, &stat_buffer, params);
    } else if(params->inode){
        printf("%lu %s  ", file_to_print->d_ino, file_to_print->d_name);
    } else {
        printf("%s  ", file_to_print->d_name);
    }
}

void print_stat(char* file_name, struct stat* file_to_print, unix_ls_arg* params){
    if(params->long_list){
        // TODO column's widths
        if(params->inode){
            printf("%lu ", file_to_print->st_ino);
        }
        struct group* grp = getgrgid(file_to_print->st_gid);
        struct passwd* pw = getpwuid(file_to_print->st_uid);
        char date_string_buffer[18];
        get_date_string(file_to_print->st_mtim, date_string_buffer);
        char mode_string_buffer[11];
        get_mode_string(file_to_print->st_mode, mode_string_buffer);
        // TODO column widths
        printf("%s %4lu %s %s %5lu %s %s\n", 
            mode_string_buffer, //Col1
            file_to_print->st_nlink, // Col2
            pw->pw_name, // Col3
            grp->gr_name, // Col4
            file_to_print->st_size, // Col5
            date_string_buffer, // Col6
            file_name // Col7
            );

    } else if (params->inode){
        printf("%lu %s  ", file_to_print->st_ino, file_name);
    } else {
        printf("%s  ", file_name);
    }
}

void get_date_string(struct timespec time_stamp, char* string_buffer){
    static const char* month_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    struct tm* pTm = localtime(&time_stamp.tv_sec);
    assert(pTm != NULL);
    // 0 to 2: month
    string_buffer[0] = month_names[pTm->tm_mon][0];
    string_buffer[1] = month_names[pTm->tm_mon][1];
    string_buffer[2] = month_names[pTm->tm_mon][2];
    string_buffer[3] = ' ';
    // 4 to 5: day
    sprintf(string_buffer+4, "%2u", pTm->tm_mday);
    string_buffer[6] = ' ';
    // 7 to 10: year
    sprintf(string_buffer+7, "%4u", pTm->tm_year + 1900);
    string_buffer[11] = ' ';
    // 12 to 13: hour
    sprintf(string_buffer+12, "%02u", pTm->tm_hour);
    string_buffer[14] = ':';
    // 15 to 16: minute
    sprintf(string_buffer+15, "%02u", pTm->tm_min);
    string_buffer[17] = '\0';
}

void get_mode_string(mode_t mode, char* string_buffer){
    // dir/file
    string_buffer[0] = S_ISDIR(mode) ? 'd' : '-';
    // Owner
    string_buffer[1] = mode & S_IRUSR ? 'r' : '-'; 
    string_buffer[2] = mode & S_IWUSR ? 'w' : '-'; 
    string_buffer[3] = mode & S_IXUSR ? 'x' : '-'; 
    // Group
    string_buffer[4] = mode & S_IRGRP ? 'r' : '-'; 
    string_buffer[5] = mode & S_IWGRP ? 'w' : '-'; 
    string_buffer[6] = mode & S_IXGRP ? 'x' : '-'; 
    // Other
    string_buffer[7] = mode & S_IROTH ? 'r' : '-'; 
    string_buffer[8] = mode & S_IWOTH ? 'w' : '-'; 
    string_buffer[9] = mode & S_IXOTH ? 'x' : '-'; 
    string_buffer[10] = '\0';
}

unsigned long string_hash(char* str){
    // This is not my hashing function 
    //  Source: https://stackoverflow.com/questions/7666509/hash-function-for-string
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

bool is_dir(char* file_name){
    struct stat file_stat;
    if(stat(file_name, &file_stat)){
        return 0;
    }
    return S_ISDIR(file_stat.st_mode);
}
