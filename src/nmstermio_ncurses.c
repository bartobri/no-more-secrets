/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

/*
 * The nmstermio_ncurses modules implements the terminal interface, with
 * ncurses support, used by the nmseffect module. All terminal IO
 * functionality is defined and implemented here.
 */

#include <string.h>
#include <ncurses.h>

// Static settings
static int clearScr          = 1;                     // clearScr flag
static int foregroundColor   = COLOR_BLUE;            // Foreground color setting

/*
 * Initialize and configure the terminal for output. This usually means
 * just turning off terminal echo, line buffering, and clearing the screen.
 * Note that ncurses always clears the screen, so there is no option to do
 * otherwise.
 */
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

/*
 * Restore the state of the terminal to the state prior to executing
 * nmstermio_init_terminal(). This usually means turning on line buffering,
 * echo, and terminal contents. 
 */
void nmstermio_restore_terminal(void) {
	endwin();
}

/*
 * Get and return the number of rows in the current terminal window.
 */
int nmstermio_get_rows(void) {
	return getmaxy(stdscr);
}

/*
 * Get and return the number of cols in the current terminal window.
 */
int nmstermio_get_cols(void) {
	return getmaxx(stdscr);
}

/*
 * Move terminal cursor to the given x/y coordinates.
 */
void nmstermio_move_cursor(int y, int x) {
	move(y, x);
}

/*
 * Print the given character string to the terminal output.
 */
void nmstermio_print_string(char *s) {
	addstr(s);
}

/*
 * Refresh the screen. Display all pending output that has not been
 * displayed yet.
 */
void nmstermio_refresh(void) {
	refresh();
}

/*
 * Clear all input pending in STDIN. This is used prior to getting user
 * input to clear the input queue in case of any prior errant keystrokes.
 */
void nmstermio_clear_input(void) {
	flushinp();
}

/*
 * Get a single character of input from the user.
 */
void nmstermio_get_char(void) {
	getch();
}

/*
 * Print the character string that represents the decrypted data. We
 * apply the bold and color attributes (if the terminal supports color)
 * to this string.
 */
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

/*
 * Display the cursor that have been turned off by nmstermio_init_terminal().
 */
void nmstermio_show_cursor(void) {
	curs_set(1);
	refresh();
}

/*
 * Make the terminal beep. Used when the returnOpts setting is set in the
 * nmseffect module.
 */
void nmstermio_beep(void) {
	beep();
}

/*
 * Return the status of the clearScr flag. This is used by the nmseffect
 * module to make certain decisions based on its value.
 */
int nmstermio_get_clearscr(void) {
	return clearScr;
}

/*
 * Sets the clearScr flag according to the true/false value of the 's' argument.
 */
void nmstermio_set_clearscr(int s) {
	if (s)
		clearScr = 1;
	else
		clearScr = 0;
}

/*
 * Set the desired foreground color of the unencrypted characters as they
 * are revealed by nmstermio_print_reveal_string(). Valid arguments are
 * "white", "yellow", "magenta", "blue", "green", "red", and "cyan".
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

/*
 * Get and returns the current row position of the cursor.
 */
int nmstermio_get_cursor_row(void) {
	return getcury(stdscr);
}
