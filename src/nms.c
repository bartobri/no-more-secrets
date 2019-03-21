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
#include <getopt.h>
#include "nmseffect.h"
#include "input.h"
#include "error.h"

#define VERSION "0.3.4"

int main(int argc, char *argv[])
{
	int r, opt;
	unsigned char *input;

	input = NULL;

	while ((opt = getopt(argc, argv, "f:aschv")) != -1)
	{
		switch (opt)
		{
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
		case 'h':
			printf("Usage : nms [-a] [-s] [-f <color>] [-c] [-h] [-v] \n");
			printf("Flags : \n");
			printf("        -a Set the auto-decrypt flag.\n");
			printf("        -s Set a flag to mask space characters.\n");
			printf("        -f <color> Set the foreground color.\n");
			printf("        -c Clear the screen prior.\n");
			printf("        -h Prints help information.\n");
			printf("        -v Display version information.\n");
			return EXIT_SUCCESS;
		case 'v':
			printf("nms version " VERSION "\n");
			return EXIT_SUCCESS;
		case '?':
			if (isprint(optopt))
			{
				error_log("Unknown option '-%c'.", optopt);
			}
			else
			{
				error_log("Unknown option character '\\x%x'.", optopt);
			}
			error_print();
			return EXIT_FAILURE;
		}
	}

	r = input_get(&input, "Enter input: ");
	if (r <= 0)
	{
		if (r == 0)
		{
			error_log("Input is empty.");
		}
		else
		{
			error_log("Could not get input.");
		}
		error_print();
		return EXIT_FAILURE;
	}

	nmseffect_exec(input, r);

	free(input);

	return EXIT_SUCCESS;
}
