/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#ifndef NMSTERM_H
#define NMSTERM_H 1

// Function prototypes
int nmsterm_init_terminal(void);
void nmsterm_restore_terminal(void);
int nmsterm_get_rows(void);
int nmsterm_get_cols(void);
int nmsterm_get_cursor_row(void);
void nmsterm_move_cursor(int, int);
void nmsterm_print_string(char *);
void nmsterm_refresh(void);
void nmsterm_clear_input(void);
char nmsterm_get_char(void);
void nmsterm_print_reveal_string(char *, int);
void nmsterm_show_cursor(void);
void nmsterm_beep(void);
int nmsterm_get_clearscr(void);
void nmsterm_set_clearscr(int);
void nmsterm_set_foregroundcolor(char *);


#endif
