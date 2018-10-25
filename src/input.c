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
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <time.h>
#include <errno.h>
#include "input.h"
#include "error.h"

int input_get(unsigned char** dest, char *prompt)
{
	int r, input_len;
	fd_set input_stream;
	struct timeval timeout;
	void *timeout_p;

	FD_ZERO(&input_stream);
	input_len = 0;

	timeout.tv_sec  = 10;
	timeout.tv_usec = 0;

	if (isatty(STDIN_FILENO))
	{
		timeout_p = NULL;
		if (prompt != NULL)
		{
			printf("%s", prompt);
			fflush(stdout);
		}
	}
	else
	{
		timeout_p = &timeout;
	}

	FD_SET(STDIN_FILENO, &input_stream);
	r = select(FD_SETSIZE, &input_stream, NULL, NULL, timeout_p);
	if (r < 0)
	{
		error_log("Error while waiting for input data. Errno: %i", errno);
		return -1;
	}
	if (r > 0)
	{
		r = ioctl(STDIN_FILENO, FIONREAD, &input_len);
		if (r < 0)
		{
			error_log("Could not determine length of input. Errno: %i", errno);
			return -1;
		}
		if (input_len > 0)
		{
			*dest = malloc(input_len);
			if (*dest == NULL)
			{
				error_log("Memory allocation error.");
				return -1;
			}
			r = read(STDIN_FILENO, *dest, input_len);
			if (r < 0)
			{
				error_log("Input read error. Errno: %i", errno);
				return -1;
			}
		}
	}

	FD_CLR(STDIN_FILENO, &input_stream);

	return input_len;
}

int input_get_str(char** dest, char *prompt)
{
	int r, i, input_len;
	unsigned char *input;

	r = input_get(&input, prompt);
	if (r < 0)
	{
		error_log("Could not get input.");
		return -1;
	}

	if (r > 0)
	{
		if (input[r - 1] == '\n')
		{
			--r;
			if (r > 0 && input[r - 1] == '\r')
			{
				--r;
			}
		}
	}

	if (r == 0)
	{
		error_log("No input provided.");
		return -1;
	}

	input_len = r;

	*dest = malloc(input_len + 1);
	if (*dest == NULL)
	{
		error_log("Memory allocation error.");
		return -1;
	}

	memset(*dest, 0, input_len + 1);

	for (i = 0; i < input_len; ++i)
	{
		if (isascii(input[i]))
		{
			(*dest)[i] = input[i];
		}
		else
		{
			error_log("Input contains non-ascii characters.");
			return -1;
		}
	}

	free(input);

	return input_len;
}

int input_get_from_pipe(unsigned char** dest)
{
	int r;

	if (isatty(STDIN_FILENO))
	{
		error_log("Input data from a piped or redirected source is required.");
		return -1;
	}

	r = input_get(dest, NULL);
	if (r < 0)
	{
		error_log("Could not get input.");
		return -1;
	}
	if (r == 0)
	{
		error_log("No input provided.");
		return -1;
	}

	return r;
}