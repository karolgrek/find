#ifndef FIND_H
#define FIND_H

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

typedef enum {
    SORT_DEFAULT,
    SORT_PATH,
    SORT_SIZE
} sort_type;

typedef struct {
    char *start_dir;       // start dir
    char *name_substr;     // -n
    uid_t user_uid;        // -u
    bool use_user_uid;     // -u
    char *perm_mask;       // -m
    int min_depth;         // -f
    int max_depth;         // -t
    bool include_hidden;   // -a
    bool null_terminate;   // -0
    sort_type sort_type;   // -s
    bool help;             // -h
} switches_info;

typedef struct {
    char *path;
    size_t size;
} found_file;

int search_directory(const char *start_path, const switches_info *sw,
    found_file **results, size_t *count, size_t *capacity);

#endif //FIND_H
