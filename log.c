#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char name[256];
    time_t mtime;
} LogFile;

int compare_mtime(const void *a, const void *b) {
    return ((LogFile*)a)->mtime - ((LogFile*)b)->mtime;
}

int main() {
    DIR *dir = opendir(".");
    if (dir == NULL) {
        perror("Failed to open directory");
        return 1;
    }

    LogFile logs[256];
    int log_count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        struct stat statbuf;
        if (stat(entry->d_name, &statbuf) != 0) {
            perror("Failed to stat file");
            return 1;
        }

        if (S_ISREG(statbuf.st_mode) && strstr(entry->d_name, ".log") != NULL) {
            strncpy(logs[log_count].name, entry->d_name, 255);
            logs[log_count].mtime = statbuf.st_mtime;
            log_count++;
        }
    }

    closedir(dir);

    qsort(logs, log_count, sizeof(LogFile), compare_mtime);

    for (int i = 0; i < log_count - 3; i++) {
        if (remove(logs[i].name) != 0) {
            perror("Failed to delete file");
            return 1;
        }
    }

    return 0;
}

//https://leanne.godbolt.org/z/MdYK38rYM

