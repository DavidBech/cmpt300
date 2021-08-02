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
#include "fifo.h"

static fifo* long_output_fifo;
static struct output_col_widths_s{
    unsigned inode_width;
    unsigned num_links_width;
    unsigned file_size_width;
    unsigned group_width;
    unsigned owner_width;
} out_col_widths;

static struct unix_ls_arg_s{
    bool inode;
    bool long_list;
    bool recursive;
} params;

struct long_output_item_s{
    long unsigned inode_num;
    long unsigned file_size;
    long unsigned num_links;
    char* owner_string;
    char* group_string;
    char* file_name_string;
    char* symbolic_dest_string;
    char mode_string[11];
    char date_string[18];
};

static void free_long_output_item(struct long_output_item_s* pItem);
static void print_long_output_fifo();

int main(int argc, char** argv){
    memset(&params, '\0', sizeof(params));
    int dir_index = 0;

    // Set the input arguments parameters
    for(int i=1; i<argc; ++i){
        if(argv[i][0] == '-'){
            for(int j=1; j<strlen(argv[i]); ++j){
                switch(argv[i][j]){
                    case('i'):
                        params.inode = true;
                        break;
                    case('l'):
                        params.long_list = true;
                        break;
                    case('R'):
                        params.recursive = true;
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
        print_file(".", false);
    } else { 
        bool header = params.recursive || (dir_index != 0 && argc - dir_index > 1);
        // Loop through the input file args printing them
        for(int i=dir_index; i<argc; ++i){
            if(i != dir_index){
                printf("\n\n");
            }
            print_file(argv[i], header);
        }
    }
    return EXIT_SUCCESS;
}

void print_file(char* file_name, bool header){
    assert(file_name != NULL);
    fifo* rec_fifo;
    if(params.recursive){
        rec_fifo = FIFO_create();
        if(rec_fifo == NULL){
            fprintf(stderr, "Out of mem\n");
            exit(EXIT_FAILURE);
        }
    }
    DIR* directory = opendir(file_name);
    struct stat stat_buf;
    if(directory){
        // Input name is directory
        if(header){
            printf("%s:\n", file_name);
        }
        struct dirent* dir_file = readdir(directory);
        while (dir_file != NULL){
            if(dir_file->d_name[0] != '.'){
                // Print the file
                print_dirent(file_name, dir_file);    

                // If recursion is enabled and the current file is a directory
                //  add to FIFO of dirs to recursivley call this method on
                if(params.recursive && is_dir(dir_file->d_name, file_name)){
                    char* full_path = append_file_to_dir(file_name, dir_file->d_name);
                    FIFO_enqueue(rec_fifo, full_path);
                }
            }
            // Get the next file to print
            dir_file = readdir(directory);
        }
        closedir(directory);
    } else if(!stat(file_name, &stat_buf)){
        // Input file_name is file
        print_stat(file_name, &stat_buf, NULL);
    } else if(!lstat(file_name, &stat_buf)){
        // Input file_name is symbolic link
        if(params.long_list){
            char link_name_buffer[PATH_MAX];
            readlink(file_name, link_name_buffer, PATH_MAX);
            print_stat(file_name, &stat_buf, link_name_buffer);
        } else {
            print_stat(file_name, &stat_buf, NULL);
        }
    } else {
        fprintf(stderr, "UnixLs: cannot access '%s': No such file or directory\n", file_name);
    }
    if(long_output_fifo){
        print_long_output_fifo();
    }
    if(params.recursive){
        char* file_to_print;
        while((file_to_print = FIFO_dequeue(rec_fifo))){
            printf("\n");
            print_file(file_to_print, header);
            free(file_to_print);
        }
        FIFO_free(rec_fifo);
    }
    return;
}

void print_dirent(char* dirname, struct dirent* file_to_print){
    if(params.long_list){
        // Call print_stat
        char* file_with_path = append_file_to_dir(dirname, file_to_print->d_name);
        struct stat stat_buffer;
        if(!stat(file_with_path, &stat_buffer)){
            print_stat(file_to_print->d_name, &stat_buffer, NULL);    
        } else if(!lstat(file_with_path, &stat_buffer)){
            char str_buffer_link_name[PATH_MAX];
            memset(&str_buffer_link_name, '\0', PATH_MAX);
            readlink(file_with_path, str_buffer_link_name, PATH_MAX);
            print_stat(file_to_print->d_name, &stat_buffer, str_buffer_link_name);
        } else {
            fprintf(stderr, "UnixLs: cannot access '%s': No such file or directory\n", file_with_path);
        }
        free(file_with_path);
    } else if(params.inode){
        printf("%lu %s\n", file_to_print->d_ino, file_to_print->d_name);
    } else {
        printf("%s\n", file_to_print->d_name);
    }
}

void print_stat(char* file_name, struct stat* file_to_print, char* symbolic_destination){
    if(params.long_list){
        if(!long_output_fifo){
            long_output_fifo = FIFO_create();
            memset(&out_col_widths, '\0', sizeof(out_col_widths));
        }
        unsigned len;
        char buffer[32];
        memset(&buffer, '\0', 32);
        struct group* grp = getgrgid(file_to_print->st_gid);
        struct passwd* pw = getpwuid(file_to_print->st_uid);
        struct long_output_item_s* pItem = malloc(sizeof(struct long_output_item_s));
        // Inode Number
        pItem->inode_num = file_to_print->st_ino;
        sprintf(buffer, "%lu", pItem->inode_num);
        len = strlen(buffer);
        if(len > out_col_widths.inode_width){
            out_col_widths.inode_width = len;
        }
        // File Size
        pItem->file_size = file_to_print->st_size;
        sprintf(buffer, "%lu", pItem->file_size);
        len = strlen(buffer);
        if(len > out_col_widths.file_size_width){
            out_col_widths.file_size_width = len;
        }

        // Number Links
        pItem->num_links = file_to_print->st_nlink;
        sprintf(buffer, "%lu", pItem->num_links);
        len = strlen(buffer);
        if(len > out_col_widths.num_links_width){
            out_col_widths.num_links_width = len;
        }

        // Group Name
        len = strlen(grp->gr_name);
        if(len > out_col_widths.group_width){
            out_col_widths.group_width = len;
        }
        pItem->group_string = malloc(len + 1);
        strcpy(pItem->group_string, grp->gr_name);
        // Owner Name
        len = strlen(pw->pw_name);
        if(len > out_col_widths.owner_width){
            out_col_widths.owner_width = len;
        }
        pItem->owner_string = malloc(len + 1);
        strcpy(pItem->owner_string, pw->pw_name);
        // File Name
        pItem->file_name_string = malloc(strlen(file_name) + 1);
        strcpy(pItem->file_name_string, file_name);
        // Symbolic Link
        if(symbolic_destination){
            pItem->symbolic_dest_string = malloc(strlen(symbolic_destination) + 1);
            strcpy(pItem->symbolic_dest_string, symbolic_destination);
        } else {
            pItem->symbolic_dest_string = NULL;
        }
        get_date_string(file_to_print->st_mtim, pItem->date_string);
        get_mode_string(file_to_print->st_mode, pItem->mode_string, symbolic_destination != NULL);
        FIFO_enqueue(long_output_fifo, pItem);
    } else if (params.inode){
        printf("%lu %s\n", file_to_print->st_ino, file_name);
    } else {
        printf("%s\n", file_name);
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

void get_mode_string(mode_t mode, char* string_buffer, bool is_link){
    // dir/file
    string_buffer[0] = is_link ? 'l' : S_ISDIR(mode) ? 'd' : '-';
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

bool is_dir(char* file_name, char* path_name){
    struct stat file_stat;        
    char str_buffer[PATH_MAX];
    str_buffer[0] = '\0';
    strcat(str_buffer, path_name);
    strcat(str_buffer, "/");
    strcat(str_buffer, file_name);
    if(stat(str_buffer, &file_stat)){
        return 0;
    }
    return S_ISDIR(file_stat.st_mode);
}

char* append_file_to_dir(char* dir_name, char* file_name){
    char* full_path;
    int len_dir = strlen(dir_name);
    int len_file = strlen(file_name);
    // one char for '\0' one char for '/'
    int size = len_dir + len_file + 2;

    full_path = malloc(size);
    full_path[0] = '\0';
    strcat(full_path, dir_name);

    if(dir_name[len_dir-1] != '/'){
        strcat(full_path, "/");
    }
    strcat(full_path, file_name);
    return full_path;   
}

static void free_long_output_item(struct long_output_item_s* pItem){
    free(pItem->owner_string);
    free(pItem->group_string);
    free(pItem->file_name_string);
    free(pItem->symbolic_dest_string);
    free(pItem);
}

static void print_long_output_fifo(){
    struct long_output_item_s* pItem;
    while((pItem = FIFO_dequeue(long_output_fifo))){
       if(params.inode){
            printf("%*lu ", out_col_widths.inode_width, pItem->inode_num); //Col0
        }
        printf("%s %*lu %*s %*s %*lu %s %s", 
            pItem->mode_string, //Col1
            out_col_widths.num_links_width, pItem->num_links, // Col2
            out_col_widths.owner_width, pItem->owner_string, // Col3
            out_col_widths.group_width, pItem->group_string, // Col4
            out_col_widths.file_size_width, pItem->file_size, // Col5
            pItem->date_string, // Col6
            pItem->file_name_string // Col7
            );
        if(pItem->symbolic_dest_string != NULL){
            printf(" -> %s\n", pItem->symbolic_dest_string);
        }else{
            printf("\n");
        }
        free_long_output_item(pItem);
    }
    FIFO_free(long_output_fifo);
    long_output_fifo = NULL;
}
