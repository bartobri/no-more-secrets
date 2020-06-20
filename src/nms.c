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

#define VERSION "0.3.4"

int main(int argc, char *argv[])
{
	int r, o;
	unsigned char *input;

	input = NULL;

	while ((o = getopt(argc, argv, "f:aschv")) != -1)
	{
		switch (o)
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
			case 'v':
				printf("nms version " VERSION "\n");
				return EXIT_SUCCESS;
			case 'h':
				printf("Usage:\nnms works on piped data. Pipe any ASCII or UTF-8 characters to it and enjoy the magic. In the below examples, I use a simple directory listing.\n");
				printf(" $ ls -l | nms\n");
				printf(" $ ls -l | nms -a       // Set auto-decrypt flag\n");
				printf(" $ ls -l | nms -s       // Set flag to mask space characters\n");
				printf(" $ ls -l | nms -f green // Set foreground color to green\n");
				printf(" $ ls -l | nms -c       // Clear screen\n");
				printf(" $ nms -h               // Display this message end exit\n");
				printf(" $ nms -v               // Display version info and exit\n\n");
				printf("Command Line Options:\n");
				printf(" -a: Set the auto-decrypt flag. This will automatically start the decryption sequence without requiring a key press.\n");
				printf(" -s: Set a flag to mask space characters. This will only mask single blank space characters. Other space characters such as tabs and newlines will not be masked.\n");
				printf(" -f <color>: Set the foreground color of the decrypted text to the color specified. Valid options are white, yellow, black, magenta, blue, green, or red. This is blue by default.\n");
				printf(" -c: Clear the screen prior to printing any output. Specifically, it saves the state of the terminal (all current output), and restores it once the effect is completed. Note that when using this option, nms requires the user to press a key before restoring the terminal.\n");
				printf(" -h: Print this message and exit.\n");
				printf(" -v: Display version info and exit.\n");
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
