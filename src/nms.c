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
#include "input.h"
#include "error.h"

#define VERSION                "0.3.3"

int main(int argc, char *argv[]) {
	int r, o;
	unsigned char *input;

	input = NULL;

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

	r = input_get(&input, "Enter input: ");
	if (r < 0)
	{
		error_log("Could not get input.");
		error_print();
		return EXIT_FAILURE;
	}
	else if (r == 0)
	{
		error_log("Input is empty.");
		error_print();
		return EXIT_FAILURE;
	}

	r = nmseffect_exec(input, r);

	free(input);

	return EXIT_SUCCESS;
}
