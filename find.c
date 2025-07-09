#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <stdint.h>

#include "find.h"
#include "utils.h"

typedef struct {
    char *path;
    int depth;
} dir_stack_entry;

int search_directory(const char *start_path, const switches_info *sw,
                     found_file **results, size_t *count, size_t *capacity) {

    size_t stack_capacity = 32;
    size_t stack_size = 0;
    dir_stack_entry *stack = malloc(stack_capacity * sizeof(dir_stack_entry));
    if (!stack) {
        fprintf(stderr, "Memory allocation failed\n");
        return EXIT_FAILURE;
    }

    stack[stack_size++] = (dir_stack_entry){strdup(start_path), 0};

    int invalid_file_dir = 0;
    bool first_iteration = true;

    while (stack_size > 0) {
        dir_stack_entry current = stack[--stack_size];

        DIR *dir = opendir(current.path);
        if (!dir) {
            fprintf(stderr, "Could not open directory %s\n", current.path);
            free(current.path);
            if (first_iteration) invalid_file_dir = 1;
            continue;
        }

        first_iteration = false;
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            const char *name = entry->d_name;

            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
                continue;
            }

            if (!sw->include_hidden && name[0] == '.') {
                continue;
            }

            char full_path[PATH_MAX];
            uint32_t path_len = snprintf(full_path, sizeof(full_path), "%s/%s", current.path, name);
            if (path_len >= sizeof(full_path)) {
                fprintf(stderr, "Path is longer than PATH_MAX: %s/%s\n", current.path, name);
                continue;
            }

            struct stat st;
            if (lstat(full_path, &st) == -1) {
                fprintf(stderr, "lstat failed: %s/%s\n", current.path, name);
                continue;
            }

            char *path_copy = strdup(full_path);
            if (!path_copy) {
                fprintf(stderr, "Memory allocation failed\n");
                continue;
            }

            if (S_ISDIR(st.st_mode)) {
                if (sw->max_depth == -1 || (current.depth + 1) <= sw->max_depth) {
                    if (stack_size >= stack_capacity) {
                        stack_capacity *= 2;
                        void *tmp = realloc(stack, stack_capacity * sizeof(dir_stack_entry));
                        if (!tmp) {
                            fprintf(stderr, "Stack realloc failed\n");
                            free(path_copy);
                            break;
                        }
                        stack = tmp;
                    }
                    stack[stack_size++] = (dir_stack_entry){path_copy, current.depth + 1};
                }
                else {
                    free(path_copy);
                }
            }
            else if (S_ISREG(st.st_mode)) {
                if ((sw->min_depth != -1 && (current.depth + 1) < sw->min_depth) ||
                    (sw->max_depth != -1 && (current.depth + 1) > sw->max_depth)) {
                    free(path_copy);
                    continue;
                }

                if (!match_options(name, &st, sw)) {
                    free(path_copy);
                    continue;
                }

                if (*count >= *capacity) {
                    *capacity = (*capacity == 0) ? 16 : (*capacity * 2);
                    void *tmp = realloc(*results, *capacity * sizeof(found_file));
                    if (!tmp) {
                        fprintf(stderr, "Realloc failed when processing '%s'\n", path_copy);
                        free(*results);
                        free(path_copy);
                        break;
                    }
                    *results = tmp;
                }

                (*results)[*count].path = path_copy;
                (*results)[*count].size = st.st_size;
                (*count)++;
            }
            else {
                free(path_copy);
            }
        }

        closedir(dir);
        free(current.path);
    }

    free(stack);
    return invalid_file_dir == 1 ? EXIT_FAILURE : EXIT_SUCCESS;
}