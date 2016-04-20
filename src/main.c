#include <stdio.h>
#include <stdlib.h>
#include "nms.h"

int main(void) {
	char *input;
	NmsArgs args = INIT_NMSARGS;

	// Geting input
	int c, inSize = 0;
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
