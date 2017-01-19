/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#ifndef NMSEFFECT_H
#define NMSEFFECT_H 1

// Function prototypes
char nmseffect_exec(char *);
void nmseffect_set_foregroundcolor(char *);
void nmseffect_set_return_opts(char *);
void nmseffect_set_auto_decrypt(int);
void nmseffect_set_clearscr(int);
void nmseffect_use_color(int);
void nmseffect_set_input_position(int, int);

#endif
