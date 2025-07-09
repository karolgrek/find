#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <pwd.h>

#include "find.h"
#include "utils.h"


/**
 * This file should only contain the main function and
 * command line arguments processing logic. You shouldn't use it
 * for any logic apart from that. Create some own .h/.c files by yourselves.
 */

int main(int argc, char *argv[])
{
	switches_info switches = {0};

	switches.sort_type = SORT_DEFAULT;
	switches.max_depth = -1;

	int opt;
	while ((opt = getopt(argc, argv, "n:u:m:f:t:s:a0h")) != -1) {
		switch (opt) {
			case 'n':
				switches.name_substr = optarg;
				break;
			case 'u': {
				struct passwd *pw = getpwnam(optarg);
				if (!pw) {
					fprintf(stderr, "Invalid user: %s\n", optarg);
					return EXIT_FAILURE;
				}
				switches.user_uid = pw->pw_uid;
				switches.use_user_uid = true;	
				break;
			}
			case 'm':
				switches.perm_mask = optarg;
				break;
			case 'f':
				switches.min_depth = atoi(optarg);
				break;
			case 't':
				switches.max_depth = atoi(optarg);
				break;
			case 's':
				if (strcmp(optarg, "f") == 0) switches.sort_type = SORT_PATH;
				else if (strcmp(optarg, "s") == 0) switches.sort_type = SORT_SIZE;
				else {
					fprintf(stderr, "Invalid arg for -s: %s\n", optarg);
					return EXIT_FAILURE;
				}
				break;
			case 'a':
				switches.include_hidden = true;
				break;
			case '0':
				switches.null_terminate = true;
				break;
			case 'h':
				switches.help = true;
				break;
			default:
				return EXIT_FAILURE;
		}
	}

	if (switches.help) {
		print_help();
		return EXIT_FAILURE;
	}
	if (optind < argc && argv[optind][0] != '-') {
		switches.start_dir = argv[optind];
	}
	else {
		switches.start_dir = ".";
	}

	found_file *results = NULL;
	size_t count = 0;
	size_t capacity = 0;

	if (search_directory(switches.start_dir, &switches, &results, &count, &capacity) == EXIT_FAILURE) {
		return EXIT_FAILURE;
	}

	if (switches.sort_type == SORT_SIZE) {
		qsort(results, count, sizeof(found_file), compare_by_size);
	}
	else if (switches.sort_type == SORT_PATH) {
		qsort(results, count, sizeof(found_file), compare_by_path);
	}
	else {
		qsort(results, count, sizeof(found_file), compare_implicit);
	}

	char ending = '\n';
	if (switches.null_terminate) {
		ending = '\0';
	}
	for (size_t i = 0; i < count; i++) {
		printf("%s%c", results[i].path, ending);
		free(results[i].path);
	}
	free(results);
	return EXIT_SUCCESS;
}
