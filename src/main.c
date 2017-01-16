/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include "nms.h"

#define VERSION                "0.3.0"
#define INPUT_GROWTH_FACTOR    2

int main(int argc, char *argv[]) {
	int c, o, inSize = 0, inCapacity = 0;
	char *input = NULL;

	// Processing command arguments
	while ((o = getopt(argc, argv, "f:ac:v")) != -1) {
		switch (o) {
			case 'f':
				nms_set_foreground_color(optarg);
				break;
			case 'a':
				nms_set_auto_decrypt(1);
				break;
			case 'c':
				nms_set_clear_scr(1);
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

	// Display characters
	c = nms_exec(input);
	
	// Print out from nms_exec if it is not null
	if (c) {
		printf("%c", c);
	}

	// Don't forget to free the allocated memory!
	free(input);

	return 0;
}
