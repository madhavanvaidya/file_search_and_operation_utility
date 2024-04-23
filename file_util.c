#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <fcntl.h>
#include <limits.h>

#define MAX_PATH_LENGTH 1024

char *extension; // Stores the extension to search for
char *target_filename; // Stores the target filename to search for
char *storage_dir; // Stores the directory for storing files
char *root_dir; // Stores the root directory for search
char *option; // Stores the operation option ("-cp" for copy, "-mv" for move)
int found = 0; // Flag to indicate if the file is found

// Callback function for file search
int search_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    const char *filename = strrchr(fpath, '/');
    if (filename == NULL)
        filename = fpath;
    else
        filename++;

    // Check if the filename matches the target filename
    if (strcmp(filename, target_filename) == 0) {
        printf("File found at: %s\n", fpath);
        found = 1; // Set found flag to true
    }

    return 0;
}

// Callback function for file search and operation
int search_and_operate(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    const char *filename = strrchr(fpath, '/');
    if (filename == NULL)
        filename = fpath;
    else
        filename++;

    // Check if the filename matches the target filename
    if (strcmp(filename, target_filename) == 0) {
        printf("File found at: %s\n", fpath);
        found = 1; // Set found flag to true

        char dest_path[PATH_MAX];
        snprintf(dest_path, sizeof(dest_path), "%s/%s", storage_dir, target_filename);

        // Check if source and destination directories are the same
        if (strcmp(root_dir, storage_dir) == 0) {
            printf("Source and destination directories cannot be the same.\n");
            exit(EXIT_FAILURE);
        }

        // Perform the operation based on the option provided
        if (strcmp(option, "-cp") == 0) {
            char command[PATH_MAX + PATH_MAX + 20]; // Maximum length for command
            snprintf(command, sizeof(command), "cp \"%s\" \"%s\"", fpath, dest_path);
            int i = system(command);
            if (i == 0){
                printf("\nFile Copied at %s Successfully!\n", dest_path);
            }
        } else if (strcmp(option, "-mv") == 0) {
            char command[PATH_MAX + PATH_MAX + 20]; // Maximum length for command
            snprintf(command, sizeof(command), "mv \"%s\" \"%s\"", fpath, dest_path);
            int i = system(command);
            if (i == 0){
                printf("\nFile Moved at %s Successfully!\n", storage_dir);
            }
        }

        // Terminate the traversal if the file is found
        return 1;
    }

    return 0;
}

// Function to create a directory if it doesn't exist
void create_directory(const char *path) {
    char *path_copy = strdup(path);
    char *dir = dirname(path_copy);
    if (mkdir(dir, 0777) == -1 && errno != EEXIST) {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
    free(path_copy);
}

// Function to add a file to a tar archive
void add_to_tar(const char *source_file, const char *tar_file) {
    char *filename = basename(source_file);
    char *fpath_copy = strdup(source_file);

    char command[MAX_PATH_LENGTH * 2 + 50];
    snprintf(command, sizeof(command), "cd %s && tar -rf %s -C %s %s", storage_dir, tar_file, dirname(fpath_copy), filename);
    
    int result = system(command);
    if (result == -1) {
        perror("system");
        exit(EXIT_FAILURE);
    }
}

// Callback function for file search and copying to tar archive
int search_and_copy(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_F) {
        char *file_extension = strrchr(fpath, '.');
        if (file_extension && strcmp(file_extension, extension) == 0) {
            printf("File found at: %s\n", fpath);
            found = 1;
            
            char dest_path[MAX_PATH_LENGTH];
            snprintf(dest_path, sizeof(dest_path), "%s/%s", storage_dir, basename(fpath));

            create_directory(dest_path);

            add_to_tar(fpath, "a1.tar");
        }
    }
    else if (typeflag == FTW_D) {
        return 0; // Skip directories
    }

    return 0;
}

// Main function
int main(int argc, char *argv[]) {

    if(argc == 3){
        char *root_dir = argv[1];
        char *filename = argv[2];

        target_filename = filename;

        // Search for the file in the directory
        if (nftw(root_dir, search_file, 20, FTW_PHYS) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }
    else if(argc ==5){
        root_dir = argv[1];
        storage_dir = argv[2];
        option = argv[3];
        target_filename = argv[4];

        // Check if options are valid
        if (strcmp(option, "-cp") != 0 && strcmp(option, "-mv") != 0) {
            fprintf(stderr, "Invalid option. Use -cp to copy or -mv to move.\n");
            exit(EXIT_FAILURE);
        }

        // If the provided root_dir is relative, make it absolute relative to home directory
        if (root_dir[0] != '/') {
            char home_dir[PATH_MAX];
            snprintf(home_dir, sizeof(home_dir), "%s/%s", getenv("HOME"), root_dir);
            root_dir = strdup(home_dir);
        }

        // If the provided storage_dir is relative, make it absolute relative to home directory
        if (storage_dir[0] != '/') {
            char home_dir[PATH_MAX];
            snprintf(home_dir, sizeof(home_dir), "%s/%s", getenv("HOME"), storage_dir);
            storage_dir = strdup(home_dir);
        }

        // Perform file search and operation
        if (nftw(root_dir, search_and_operate, 20, FTW_PHYS) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }
    else if(argc == 4){
        root_dir = argv[1];
        storage_dir = argv[2];
        extension = argv[3];

        // If the provided root_dir is relative, make it absolute relative to home directory
        if (root_dir[0] != '/') {
            char home_dir[MAX_PATH_LENGTH];
            snprintf(home_dir, sizeof(home_dir), "%s/%s", getenv("HOME"), root_dir);
            root_dir = strdup(home_dir);
        }

        // If the provided storage_dir is relative, make it absolute relative to home directory
        if (storage_dir[0] != '/') {
            char home_dir[MAX_PATH_LENGTH];
            snprintf(home_dir, sizeof(home_dir), "%s/%s", getenv("HOME"), storage_dir);
            storage_dir = strdup(home_dir);
        }

        // Create storage directory if it doesn't exist
        if (access(storage_dir, F_OK) == -1) {
            create_directory(storage_dir);
        }

        // Perform file search and operation
        if (nftw(root_dir, search_and_copy, 20, FTW_PHYS) == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
    }
    else{
        fprintf(stderr, "Please provide correct arguments!\n");
        exit(EXIT_FAILURE);
    }

    if (!found)
        printf("Search Unsuccessful\n");

    return 0;
}
