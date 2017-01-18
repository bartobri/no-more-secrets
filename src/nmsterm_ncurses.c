/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#include <ncurses.h>

// Static settings
static int clearScr = 1;                                 // clearScr flag

// Initialize terminal window
int nmsterm_init_terminal(int foregroundColor, int c) {
	(void) c;

	initscr();
	cbreak();
	noecho();
	curs_set(0);
	move(0, 0);
	if (has_colors()) {
		start_color();
		init_pair(1, foregroundColor, COLOR_BLACK);
	}
	
	// Since ncurses always clears the screen, we always return zero.
	return 0;
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

void nmsterm_print_reveal_string(char *s, int foregroundColor, int colorOn) {
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
