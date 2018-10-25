/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GPL License. See LICENSE for more details.
 */

#ifndef ERROR_H
#define ERROR_H 1

void error_log(char *, ...);
void error_print(void);
char *error_get(void);
void error_clear(void);

#endif