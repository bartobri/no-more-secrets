// Copyright (c) 2016 Brian Barto
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.  See COPYING for more details.

#include "nms.h"
#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define TAB_SIZE             4
#define TYPE_EFFECT_SPEED    4     // miliseconds per char
#define JUMBLE_SECONDS       2     // number of seconds for jumble effect
#define JUMBLE_LOOP_SPEED    35    // miliseconds between each jumble
#define REVEAL_LOOP_SPEED    100   // miliseconds (must be evenly divisible by 5)

#define SPACE      32
#define NEWLINE    10
#define TAB        9

// Window position structure, linked list. Keeps track of every
// character's position on the terminal, as well as other attributes.
struct winpos {
	char source;
	char mask;
	int row;
	int col;
	int s1_time;
	int s2_time;
	struct winpos *next;
};

// Function prototypes (internal)
char getMaskChar(void);

/*
 * void nms_exec(NmsArgs *)
 *
 * DESCR:
 * Performs "decryption" effect, Returns the character pressed at the last pause, or
 * '\0' (null character) if there are any problems.
 *
 * ARGS:
 * NmsArgs *args - Pointer to argument structure
 *
 *      STRUCTURE MEMBERS:
 *      args.src - Pointer to string on which to perform the effect
 *      args.return_opts - Pointer to string containing character options for menu
 *      args.input_cursor_x - X screen coordinate to place cursor after "decryption"
 *      args.input_cursor_y - Y screen coordinate to place cursor after "decryption"
 *      args.show_cursor - 'true' to display cursor, 'false' to hide cursor.
 *
 */
char nms_exec(NmsArgs *args) {
	struct winpos *list_pointer = NULL;
	struct winpos *start;                   // Always points to start of list
	struct winpos *temp;                    // Used for free()ing the list
	int termSizeRows, termSizeCols;
	int c, n, x = 0, y = 0;
	int r_time, r_time_l, r_time_s;
	bool first = true;
	char ret = 0;

	// Lets check args.src and make sure we have text. If not, return
	// with an error message.
	if (args->src == NULL) {
		fprintf(stderr, "Error. No data found in args.src\n");
		return 0;
	}

	// Start and initialize curses mode
	initscr();
	cbreak();
	noecho();
	scrollok(stdscr, true);
	if (args->show_cursor == false)
		curs_set(0);

	// Setting up and starting colors if terminal supports them
	if (has_colors()) {
		start_color();
		init_pair(1, COLOR_BLUE, COLOR_BLACK);
	}

	// Get terminal window size
	getmaxyx(stdscr, termSizeRows, termSizeCols);

	// Seed my random number generator with the current time
	srand(time(NULL));

	// Geting input
	n = 0;
	while ((c = args->src[n++]) != '\0') {
		if (c == NEWLINE) {
			++y;
			x = 0;
		} else if (c == TAB && x + TAB_SIZE <= termSizeCols) {
			x += TAB_SIZE;
		} else if (isspace(c)) {
			if (++x > termSizeCols) {
				++y;
				x = 0;
			}
		} else {
			if (first) {
				list_pointer = malloc(sizeof(struct winpos));
				start = list_pointer;
				first = false;
			} else {
				list_pointer->next = malloc(sizeof(struct winpos));
				list_pointer = list_pointer->next;
			}

			r_time = rand() % 50;
			r_time_s = r_time * .25;
			r_time_l = r_time * .75;
			r_time *= 100;
			r_time_s *= 100;
			r_time_l *= 100;

			list_pointer->source = c;
			list_pointer->mask = getMaskChar();
			list_pointer->row = y;
			list_pointer->col = x;
			list_pointer->s1_time = r_time > 1000 ? r_time_l : r_time;
			list_pointer->s2_time = r_time > 1000 ? r_time_s : 0;
			list_pointer->next = NULL;

			if (++x > termSizeCols) {
				++y;
				x = 0;
			}
		}
	}

	// Initially display the characters in the terminal with a 'type effect'.
	list_pointer = start;
	while (list_pointer != NULL && list_pointer->row <= termSizeRows) {
		mvaddch(list_pointer->row, list_pointer->col, list_pointer->mask);
		refresh();
		list_pointer->mask = getMaskChar();
		list_pointer = list_pointer->next;
		usleep(TYPE_EFFECT_SPEED * 1000);
	}

	// Flush any input up to this point
	flushinp();

	// If auto_decrypt flag is set, we sleep. Otherwise, reopen stdin for interactive
	// input (keyboard), then require user to press a key to continue.
	if (args->auto_decrypt == true || (!isatty(STDIN_FILENO) && !freopen ("/dev/tty", "r", stdin)))
		sleep(1);
	else
		getch();

	// Jumble loop
	x = 0;
	while (x < (JUMBLE_SECONDS * 1000) / JUMBLE_LOOP_SPEED) {
		list_pointer = start;
		while (list_pointer != NULL && list_pointer->row <= termSizeRows) {
			mvaddch(list_pointer->row, list_pointer->col, list_pointer->mask);
			refresh();
			list_pointer->mask = getMaskChar();
			list_pointer = list_pointer->next;
		}
		usleep(JUMBLE_LOOP_SPEED * 1000);
		++x;
	}


	// Reveal loop
	int s1_remask_time = 500;     // time, in milliseconds, we change the mask for stage 1
	bool loop = true;
	while (loop) {
		loop = false;
		list_pointer = start;
		while (list_pointer != NULL && list_pointer->row <= termSizeRows) {
			if (list_pointer->s1_time > 0) {
				loop = true;
				list_pointer->s1_time -= REVEAL_LOOP_SPEED;
				if (list_pointer->s1_time % s1_remask_time == 0) {
					list_pointer->mask = getMaskChar();
				}
			} else if (list_pointer->s2_time > 0) {
				loop = true;
				list_pointer->s2_time -= REVEAL_LOOP_SPEED;
				list_pointer->mask = getMaskChar();
			} else {
				list_pointer->mask = list_pointer->source;
				attron(A_BOLD);
				if (has_colors())
					attron(COLOR_PAIR(1));
			}
			mvaddch(list_pointer->row, list_pointer->col, list_pointer->mask);
			refresh();
			list_pointer = list_pointer->next;

			attroff(A_BOLD);
			if (has_colors())
				attroff(COLOR_PAIR(1));
		}
		usleep(REVEAL_LOOP_SPEED * 1000);
	}

	// Printing remaining characters from list if we stopped short due to 
	// a terminal row limitation. i.e. the number of textual rows in the input
	// stream were greater than the number of rows in the terminal.
	int prevRow;
	if (list_pointer != NULL) {
		attron(A_BOLD);
		if (has_colors())
			attron(COLOR_PAIR(1));
		prevRow = list_pointer->row;
		scroll(stdscr);
		while (list_pointer != NULL) {
			while (list_pointer->row > prevRow) {
				scroll(stdscr);
				++prevRow;
			}
			mvaddch(termSizeRows -1, list_pointer->col, list_pointer->source);
			refresh();
			//mvaddch(0, 0, list_pointer->source);
			list_pointer = list_pointer->next;
		}
		attroff(A_BOLD);
		if (has_colors())
			attroff(COLOR_PAIR(1));
	}

	// Flush any input up to this point
	flushinp();

	// Position cursor
	if (args->input_cursor_y >= 0 && args->input_cursor_x >= 0) {
		move(args->input_cursor_y, args->input_cursor_x);
		curs_set(1);
	}

	// If stdin is set to the keyboard, user must press a key to continue
	if (isatty(STDIN_FILENO)) {
		ret = getch();
		if (args->return_opts != NULL && strlen(args->return_opts) > 0)
			while (index(args->return_opts, ret) == NULL) {
				beep();
				ret = getch();
			}
	} else
		sleep(2);


	// End curses mode
	endwin();

	// Freeing the list. 
	list_pointer = start;
	while (list_pointer != NULL) {
		temp = list_pointer;
		list_pointer = list_pointer->next;
		free(temp);
	}

	return ret;
}

/*
 * char getMaskChar(void)
 *
 * DESCR:
 * Returns a random character from the maskChars string. 
 *
 */
char getMaskChar(void) {
	char *maskChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	                  "abcdefghijklmnopqrstuvwxyz"
	                  "1234567890"
	                  "!@#$%^&*()-_=+{}[]:;|\\\"'<>,.?/";

	return maskChars[rand() % strlen(maskChars)];
}
