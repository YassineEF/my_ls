#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <getopt.h>

#define MAX_PATH_LENGTH 4096

// Function prototypes
void list_files_directories(const char *path, int list_all, int details);
void print_details(const char *filename);
char *get_file_permissions(mode_t mode);
void print_error(const char *message);

int main(int argc, char *argv[]){
    int options;
    int list_all = 0;
    int details = 0;
    char *directory = ".";

    // Parse command-line options
    while ((options = getopt(argc, argv, "lRadtrAL")) != -1) {
        switch (options) {
            case 'l':
                details = 1;
                break;
            case 'R':
            case 'r':
            case 'd':
            case 't':
            case 'a':
            case 'A':
            case 'L':
                // Options other than 'l' are not implemented
                printf("Option -%c is not implemented.\n", options);
                return 1;
            default:
                fprintf(stderr, "Usage: %s [-l] [directory]\n", argv[0]);
                return 1;
        }
    }

    // If a directory is provided as an argument, use it
    if (optind < argc) {
        directory = argv[optind];
    }

    // List the directory
    list_files_directories(directory, list_all, details);

    printf("\n");

    return 0; 
}


void list_files_directories(const char *path, int list_all, int details) {
        DIR *dir;
        struct dirent *entry;

        if ((dir = opendir(path)) == NULL) {
            print_error("opendir");
            return;
        }

        while ((entry = readdir(dir)) != NULL) {
            // Skip hidden files if not listing all files
            if (!list_all && entry->d_name[0] == '.') {
                continue;
            }

            // Print in long format if requested
            if (details) {
                print_details(entry->d_name);
            } 
            //if the entry is the file
            else if(entry->d_type == DT_REG){
                printf("%s\t", entry->d_name);
            }
            //if the entry is a directory
            else if(entry->d_type == DT_DIR){
                printf("\033[0;34m%s\033[0m\t", entry->d_name);
            }
        }

        closedir(dir);
}

// Print file information in long format
void print_details(const char *filename) {
    struct stat file_stat;
    char full_path[MAX_PATH_LENGTH];

    if (stat(filename, &file_stat) == -1) {
        print_error("stat");
        return;
    }

    // Print permissions
    printf("%s ", get_file_permissions(file_stat.st_mode));

    // Print number of links
    printf("%ld ", (long)file_stat.st_nlink);

    // Print owner name
    struct passwd *owner_info = getpwuid(file_stat.st_uid);
    if (owner_info != NULL) {
        printf("%s ", owner_info->pw_name);
    } else {
        printf("%d ", file_stat.st_uid);
    }

    // Print group name
    struct group *group_info = getgrgid(file_stat.st_gid);
    if (group_info != NULL) {
        printf("%s ", group_info->gr_name);
    } else {
        printf("%d ", file_stat.st_gid);
    }

    // Print file size
    printf("%ld ", (long)file_stat.st_size);

    // Print last modified time
    struct tm *time_info = localtime(&file_stat.st_mtime);
    char time_buffer[80];
    strftime(time_buffer, 80, "%b %d %H:%M", time_info);
    printf("%s ", time_buffer);

    // Print file name
    printf("%s\n", filename);
}

// Get file permissions string
char *get_file_permissions(mode_t mode) {
    static char permissions[11];
    permissions[0] = (S_ISDIR(mode)) ? 'd' : '-';
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}

// Print error message
void print_error(const char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
}
