#include <stdio.h>
#include <stdlib.h>
#include "nms.h"

int main(void) {
	char *input;

	// Geting input
	int c, inSize = 0;
	while ((c = getchar()) != EOF) {
		++inSize;
		input = realloc(input, inSize + 1);
		input[inSize - 1] = c;
		input[inSize] = '\0';
	}

	// Display characters
	nmsexec(input);

	// Don't forget to free the allocated memory!
	free(input);

	return 0;
}
