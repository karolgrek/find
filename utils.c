#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

void print_help() {
    fprintf(stderr,
        "Usage: ./find [OPTION]... PATH\n"
            "Finds files (much like find) in specified directory\n"
            "\n"
            "PATH    argument specifies which path should be processed\n"
            "\n"
            "OPTION:\n"
            "-n NAME specifies name of file to be found\n"
            "-s [f|s] modifies print order, `f` means by path, `s` by size\n"
            "-m MASK enables file searching by FS permissions applied\n"
            "-u USER searches for files owned by USER\n"
            "-f LLVL searches files in distance at least MLVL\n"
            "-t TLVL searches files in distance at most TLVL\n"
            "-a      enables searching for hidden files as well\n");
}

int compare_implicit(const void *a, const void *b) {
    const found_file *file_a = a;
    const found_file *file_b = b;

    const char *name_a = strrchr(file_a->path, '/');
    const char *name_b = strrchr(file_b->path, '/');

    name_a = name_a ? name_a + 1 : file_a->path;
    name_b = name_b ? name_b + 1 : file_b->path;

    int name_cmp = strcasecmp(name_a, name_b);
    if (name_cmp != 0) return name_cmp;

    return strcmp(file_a->path, file_b->path);
}

int compare_by_path(const void *a, const void *b) {
    const found_file *file_a = a;
    const found_file *file_b = b;
    return strcmp(file_a->path, file_b->path);
}

int compare_by_size(const void *a, const void *b) {
    const found_file *file_a = a;
    const found_file *file_b = b;

    if (file_a->size > file_b->size) return -1;
    if (file_a->size < file_b->size) return 1;
    return compare_implicit(a, b);
}

bool match_options(const char *name, const struct stat *st, const switches_info *si) {
    if (si->name_substr && !strstr(name, si->name_substr)) {
        return false;
    }

    if (si->use_user_uid && st->st_uid != (uid_t) si->user_uid) {
        return false;
    }

    if (si->perm_mask) {
        char *endptr;
        unsigned long mask = strtol(si->perm_mask, &endptr, 8);

        if (*endptr != '\0') {
            return false;
        }

        if ((st->st_mode & 0777) != mask) {
            return false;
        }
    }
    return true;
}