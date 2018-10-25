/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GPL License. See LICENSE for more details.
 */

#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>

#define ERROR_LIST_MAX		20
#define ERROR_LENGTH_MAX	100

static char error_stack[ERROR_LIST_MAX][ERROR_LENGTH_MAX];
static int N = 0;

void error_log(char *error, ...)
{
	va_list argList;

	if (N < ERROR_LIST_MAX)
	{
		va_start(argList, error);
		vsnprintf(error_stack[N++], ERROR_LENGTH_MAX - 1, error, argList);
		va_end(argList);
	}
}

void error_print(void)
{
	int i;

	for (i = N-1; i >= 0; --i)
	{
		fprintf(stderr, "%s ", error_stack[i]);
	}
	fprintf(stderr, "\n");
}

char *error_get(void)
{
	if (N > 0)
	{
		return error_stack[--N];
	}
	else
	{
		return NULL;
	}
}

void error_clear(void)
{
	N = 0;
}