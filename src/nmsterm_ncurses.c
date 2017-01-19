/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#include <string.h>
#include <ncurses.h>

// Static settings
// Static settings
static int clearScr          = 1;                     // clearScr flag
static int foregroundColor   = COLOR_BLUE;            // Foreground color setting

// Initialize terminal window
void nmsterm_init_terminal(void) {
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	move(0, 0);
	if (has_colors()) {
		start_color();
		init_pair(1, foregroundColor, COLOR_BLACK);
	}
}

void nmsterm_restore_terminal(void) {
	endwin();
}

/*
 * nms_term_rows() gets and returns the number of rows in the current
 * terminal window.
 */
int nmsterm_get_rows(void) {
	return getmaxy(stdscr);
}

/*
 * nms_term_cols() gets and returns the number of cols in the current
 * terminal window.
 */
int nmsterm_get_cols(void) {
	return getmaxx(stdscr);
}

/*
 * nms_get_cursor_row() returns the row position of the cursor
 */
int nmsterm_get_cursor_row(void) {
	return getcury(stdscr);
}

void nmsterm_move_cursor(int y, int x) {
	move(y, x);
}

void nmsterm_print_string(char *s) {
	addstr(s);
}

void nmsterm_refresh(void) {
	refresh();
}

void nmsterm_clear_input(void) {
	flushinp();
}

void nmsterm_get_char(void) {
	getch();
}

void nmsterm_print_reveal_string(char *s, int colorOn) {
	(void) colorOn;
	(void) foregroundColor;

	// Set bold and foreground color for character reveal
	attron(A_BOLD);
	if (has_colors())
		attron(COLOR_PAIR(1));
	
	// print source character
	addstr(s);
	
	// Unset foreground color
	if (has_colors())
		attroff(COLOR_PAIR(1));
	attroff(A_BOLD);
}

void nmsterm_show_cursor(void) {
	curs_set(1);
	refresh();
}

void nmsterm_beep(void) {
	beep();
}

int nmsterm_get_clearscr(void) {
	return clearScr;
}

/*
 * nmsterm_set_clearscr() sets the clearScr flag according to the
 * true/false value of the 's' argument.
 */
void nmsterm_set_clearscr(int s) {
	if (s)
		clearScr = 1;
	else
		clearScr = 0;
}

/*
 * nms_set_foreground_color() sets the foreground color of the unencrypted
 * characters as they are revealed to the color indicated by the 'color'
 * argument. Valid arguments are "white", "yellow", "magenta", "blue",
 * "green", "red", and "cyan". This function will default to blue if
 * passed an invalid color. No value is returned.
 */
void nmsterm_set_foregroundcolor(char *c) {

	if(strcmp("white", c) == 0)
		foregroundColor =  COLOR_WHITE;
	else if(strcmp("yellow", c) == 0)
		foregroundColor = COLOR_YELLOW;
	else if(strcmp("black", c) == 0)
		foregroundColor = COLOR_BLACK;
	else if(strcmp("magenta", c) == 0)
		foregroundColor = COLOR_MAGENTA;
	else if(strcmp("blue", c) == 0)
		foregroundColor = COLOR_BLUE;
	else if(strcmp("green", c) == 0)
		foregroundColor = COLOR_GREEN;
	else if(strcmp("red", c) == 0)
		foregroundColor = COLOR_RED;
	else if(strcmp("cyan", c) == 0)
		foregroundColor = COLOR_CYAN;
	else
		foregroundColor = COLOR_BLUE;
}
