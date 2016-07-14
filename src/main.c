#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "nms.h"

#define VERSION                "0.2.1"
#define INPUT_GROWTH_FACTOR    2

int main(int argc, char *argv[]) {
	int c, o, inSize = 0, inCapacity = 0;
	char *input = NULL;
	NmsArgs args = INIT_NMSARGS;

	// Processing command arguments
	while ((o = getopt(argc, argv, "f:av")) != -1) {
		switch (o) {
			case 'f':
				args.foreground_color = optarg;
				break;
			case 'a':
                args.auto_decrypt = true;
                break;
			case 'v':
				printf("nms version " VERSION "\n");
				return 0;
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
		if (inSize > inCapacity) {
			inCapacity = inCapacity == 0 ? INPUT_GROWTH_FACTOR : inCapacity * INPUT_GROWTH_FACTOR;
			input = realloc(input, inCapacity + 1);
		}
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
