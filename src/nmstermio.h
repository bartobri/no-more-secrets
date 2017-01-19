/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#ifndef NMSTERMIO_H
#define NMSTERMIO_H 1

// Function prototypes
void nmstermio_init_terminal(void);
void nmstermio_restore_terminal(void);
int nmstermio_get_rows(void);
int nmstermio_get_cols(void);
int nmstermio_get_cursor_row(void);
void nmstermio_move_cursor(int, int);
void nmstermio_print_string(char *);
void nmstermio_refresh(void);
void nmstermio_clear_input(void);
char nmstermio_get_char(void);
void nmstermio_print_reveal_string(char *, int);
void nmstermio_show_cursor(void);
void nmstermio_beep(void);
int nmstermio_get_clearscr(void);
void nmstermio_set_clearscr(int);
void nmstermio_set_foregroundcolor(char *);
int nmstermio_get_cursor_row(void);


#endif
