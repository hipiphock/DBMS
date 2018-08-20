#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <inttypes.h>
#include "bpt.h"
#include "file.h"

// MAIN
int main( int argc, char ** argv ) {
	uint64_t input_key;
    char input_value[VALUE_SIZE];
	char instruction;

    license_notice();
	usage_1();  
	usage_2();

    open_db("test.db");
	printf("> ");
	while (scanf("%c", &instruction) != EOF) {
		switch (instruction) {
		case 'i':
			scanf("%" PRIu64 " %s", &input_key, input_value);
			insert(input_key, input_value);
			print_tree();
			break;
        case 'd':
			scanf("%" PRIu64 "", &input_key);
			delete(input_key);
			print_tree();
			break;
		case 'f':
		case 'p':
			scanf("%" PRIu64 "", &input_key);
			find_and_print(input_key);
			break;
		case 'q':
			while (getchar() != (int)'\n');
			return EXIT_SUCCESS;
			break;
		case 't':
			print_tree();
			break;
        default:
			usage_2();
			break;
		}
		while (getchar() != (int)'\n');
		printf("> ");
	}
	printf("\n");

    close_db();

	return EXIT_SUCCESS;
}
