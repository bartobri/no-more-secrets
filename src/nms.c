/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GPL License. See LICENSE for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "nmseffect.h"

#define VERSION                "0.3.3"
#define INITIAL_CAPACITY       50
#define INPUT_GROWTH_FACTOR    2

int main(int argc, char *argv[]) {
	int c, o, i, inCapacity = INITIAL_CAPACITY;
	char *input = NULL;

	// Processing command arguments
	while ((o = getopt(argc, argv, "f:ascv")) != -1) {
		switch (o) {
			case 'f':
				nmseffect_set_foregroundcolor(optarg);
				break;
			case 'a':
				nmseffect_set_autodecrypt(1);
				break;
            case 's':
                nmseffect_set_maskblank(1);
                break;
			case 'c':
				nmseffect_set_clearscr(1);
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
	
	// Allocate memory for our input buffer
	if ((input = malloc(inCapacity + 1)) == NULL) {
		fprintf (stderr, "Memory Allocation Error! Quitting...\n");
		return 1;
	}

	// Geting input
	for (i = 0; (c = getchar()) != EOF; ++i) {
		if (i >= inCapacity) {
			inCapacity *= INPUT_GROWTH_FACTOR;
			input = realloc(input, inCapacity + 1);
			if (input == NULL) {
				fprintf (stderr, "Memory Allocation Error! Quitting...\n");
				return 1;
			}
		}
		input[i] = c;
		input[i+1] = '\0';
	}

	// Execute effect
	c = nmseffect_exec(input);

	// Free allocated memory (not necessary here, but good practice)
	free(input);

	return 0;
}
