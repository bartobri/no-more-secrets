/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#include <string.h>
#include <ncurses.h>

// Static settings
static int clearScr          = 1;                     // clearScr flag
static int foregroundColor   = COLOR_BLUE;            // Foreground color setting

// Initialize terminal window
void nmstermio_init_terminal(void) {
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

void nmstermio_restore_terminal(void) {
	endwin();
}

/*
 * Gets and returns the number of rows in the current
 * terminal window.
 */
int nmstermio_get_rows(void) {
	return getmaxy(stdscr);
}

/*
 * Gets and returns the number of cols in the current
 * terminal window.
 */
int nmstermio_get_cols(void) {
	return getmaxx(stdscr);
}

/*
 * Returns the row position of the cursor
 */
int nmstermio_get_cursor_row(void) {
	return getcury(stdscr);
}

void nmstermio_move_cursor(int y, int x) {
	move(y, x);
}

void nmstermio_print_string(char *s) {
	addstr(s);
}

void nmstermio_refresh(void) {
	refresh();
}

void nmstermio_clear_input(void) {
	flushinp();
}

void nmstermio_get_char(void) {
	getch();
}

void nmstermio_print_reveal_string(char *s, int colorOn) {
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

void nmstermio_show_cursor(void) {
	curs_set(1);
	refresh();
}

void nmstermio_beep(void) {
	beep();
}

int nmstermio_get_clearscr(void) {
	return clearScr;
}

/*
 * Sets the clearScr flag according to the
 * true/false value of the 's' argument.
 */
void nmstermio_set_clearscr(int s) {
	if (s)
		clearScr = 1;
	else
		clearScr = 0;
}

/*
 * Sets the foreground color of the unencrypted
 * characters as they are revealed to the color indicated by the 'color'
 * argument. Valid arguments are "white", "yellow", "magenta", "blue",
 * "green", "red", and "cyan". This function will default to blue if
 * passed an invalid color. No value is returned.
 */
void nmstermio_set_foregroundcolor(char *c) {

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
