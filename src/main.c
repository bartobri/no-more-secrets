#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "nms.h"

#define INPUT_GROWTH_FACTOR    2

int main(int argc, char *argv[]) {
	int c, inSize = 0, inCapacity = 0;
	char *input = NULL;
	NmsArgs args = nms_getopt(argc, argv);

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
