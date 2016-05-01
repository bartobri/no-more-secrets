#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "nms.h"

int main(int argc, char *argv[]) {
	int c, o, inSize = 0;
	char *input = NULL;
	NmsArgs args = INIT_NMSARGS;

	// Processing command arguments
	while ((o = getopt(argc, argv, "a")) != -1) {
		switch (o) {
			case 'a':
                args.auto_decrypt = true;
                break;
			case '?':
				if (isprint(optopt))
					fprintf (stderr, "Unknown option '-%c'.\n", optopt);
				else
					fprintf (stderr, "Unknown option character '\\x%x'.\n", optopt);
                return 1;
        }
    }

	// Geting input
	while ((c = getchar()) != EOF) {
		++inSize;
		input = realloc(input, inSize + 1);
		input[inSize - 1] = c;
		input[inSize] = '\0';
	}

	// Set needed args
	args.src = input;
	args.show_cursor = true;

	// Display characters
	nms_exec(&args);

	// Don't forget to free the allocated memory!
	free(input);

	return 0;
}
