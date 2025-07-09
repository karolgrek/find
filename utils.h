#include <stdbool.h>
#include "find.h"
#include <sys/stat.h>

#ifndef UTILS_H
#define UTILS_H

void print_help();
bool match_options(const char *name, const struct stat *st, const switches_info *sw);
int compare_implicit(const void *a, const void *b);
int compare_by_path(const void *a, const void *b);
int compare_by_size(const void *a, const void *b);

#endif //UTILS_H
