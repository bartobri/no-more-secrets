// Copyright (c) 2016 Brian Barto
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 3 of the License, or (at your option)
// any later version.  See COPYING for more details.

#include "nms.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <ncurses.h>

#define TYPE_EFFECT_SPEED    4     // miliseconds per char
#define JUMBLE_SECONDS       2     // number of seconds for jumble effect
#define JUMBLE_LOOP_SPEED    35    // miliseconds between each jumble
#define REVEAL_LOOP_SPEED    100   // miliseconds (must be evenly divisible by 5)

#define SPACE      32
#define NEWLINE    10
#define TAB        9
#define TAB_SIZE   4

// Window position structure, linked list. Keeps track of every
// character's position on the terminal, as well as other attributes.
struct winpos {
	char *source;
	char *mask;
	int row;
	int col;
	int s1_time;
	int s2_time;
	struct winpos *next;
};

// Function prototypes (internal)
char *getMaskChar(void);
int getColorByName(char *);

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

	// Seems to be needed for utf-8 support
	setlocale(LC_ALL, "");

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
		init_pair(1, getColorByName(args->foreground_color), COLOR_BLACK);
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

			if ((c & 0x80) == 0x00) {
				// c is an ascii character
				list_pointer->source = malloc(2);
				list_pointer->source[0] = c;
				list_pointer->source[1] = '\0';
			} else if ((c & 0xE0) == 0xC0) {
				// 2 byte char
				list_pointer->source = malloc(3);
				list_pointer->source[0] = c;
				list_pointer->source[1] = args->src[n++];
				list_pointer->source[2] = '\0';
			} else if ((c & 0xF0) == 0xE0) {
				// 3 byte char
				list_pointer->source = malloc(4);
				list_pointer->source[0] = c;
				list_pointer->source[1] = args->src[n++];
				list_pointer->source[2] = args->src[n++];
				list_pointer->source[3] = '\0';
			} else if ((c & 0xF8) == 0xF0) {
				// 4 byte char
				list_pointer->source = malloc(5);
				list_pointer->source[0] = c;
				list_pointer->source[1] = args->src[n++];
				list_pointer->source[2] = args->src[n++];
				list_pointer->source[3] = args->src[n++];
				list_pointer->source[4] = '\0';
			} else {
				// Unrecognized char, treat it as a single-byte char
				list_pointer->source = malloc(2);
				list_pointer->source[0] = c;
				list_pointer->source[1] = '\0';
			}
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
		mvaddstr(list_pointer->row, list_pointer->col, list_pointer->mask);
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
			mvaddstr(list_pointer->row, list_pointer->col, list_pointer->mask);
			list_pointer->mask = getMaskChar();
			list_pointer = list_pointer->next;
		}
		refresh();
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
			mvaddstr(list_pointer->row, list_pointer->col, list_pointer->mask);
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
			mvaddstr(termSizeRows -1, list_pointer->col, list_pointer->source);
			refresh();
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
		free(temp->mask);
		free(temp);
	}

	return ret;
}

/*
 * char *getMaskChar(void)
 *
 * DESCR:
 * Returns a random character from the maskChars string. 
 *
 */
char *getMaskChar(void) {
	char *maskChars[] = {
		"!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", "~",
		".", "/", ":", ";", "<", "=", ">", "?", "[", "\\", "]", "_", "{", "}",
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
		"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
		"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"\xc3\x87", "\xc3\xbc", "\xc3\xa9", "\xc3\xa2", "\xc3\xa4", "\xc3\xa0",
		"\xc3\xa5", "\xc3\xa7", "\xc3\xaa", "\xc3\xab", "\xc3\xa8", "\xc3\xaf",
		"\xc3\xae", "\xc3\xac", "\xc3\x84", "\xc3\x85", "\xc3\x89", "\xc3\xa6",
		"\xc3\x86", "\xc3\xb4", "\xc3\xb6", "\xc3\xb2", "\xc3\xbb", "\xc3\xb9",
		"\xc3\xbf", "\xc3\x96", "\xc3\x9c", "\xc2\xa2", "\xc2\xa3", "\xc2\xa5",
		"\xc6\x92", "\xc3\xa1", "\xc3\xad", "\xc3\xb3", "\xc3\xba", "\xc3\xb1",
		"\xc3\x91", "\xc2\xaa", "\xc2\xba", "\xc2\xbf", "\xc2\xac", "\xc2\xbd",
		"\xc2\xbc", "\xc2\xa1", "\xc2\xab", "\xc2\xbb", "\xce\xb1", "\xc3\x9f",
		"\xce\x93", "\xcf\x80", "\xce\xa3", "\xcf\x83", "\xc2\xb5", "\xcf\x84",
		"\xce\xa6", "\xce\x98", "\xce\xa9", "\xce\xb4", "\xcf\x86", "\xce\xb5",
		"\xc2\xb1", "\xc3\xb7", "\xc2\xb0", "\xc2\xb7", "\xc2\xb2", "\xc2\xb6",
		"\xe2\x8c\x90", "\xe2\x82\xa7", "\xe2\x96\x91", "\xe2\x96\x92",
		"\xe2\x96\x93", "\xe2\x94\x82", "\xe2\x94\xa4", "\xe2\x95\xa1",
		"\xe2\x95\xa2", "\xe2\x95\x96", "\xe2\x95\x95", "\xe2\x95\xa3",
		"\xe2\x95\x91", "\xe2\x95\x97", "\xe2\x95\x9d", "\xe2\x95\x9c",
		"\xe2\x95\x9b", "\xe2\x94\x90", "\xe2\x94\x94", "\xe2\x94\xb4",
		"\xe2\x94\xac", "\xe2\x94\x9c", "\xe2\x94\x80", "\xe2\x94\xbc",
		"\xe2\x95\x9e", "\xe2\x95\x9f", "\xe2\x95\x9a", "\xe2\x95\x94",
		"\xe2\x95\xa9", "\xe2\x95\xa6", "\xe2\x95\xa0", "\xe2\x95\x90",
		"\xe2\x95\xac", "\xe2\x95\xa7", "\xe2\x95\xa8", "\xe2\x95\xa4",
		"\xe2\x95\xa7", "\xe2\x95\x99", "\xe2\x95\x98", "\xe2\x95\x92",
		"\xe2\x95\x93", "\xe2\x95\xab", "\xe2\x95\xaa", "\xe2\x94\x98",
		"\xe2\x94\x8c", "\xe2\x96\x88", "\xe2\x96\x84", "\xe2\x96\x8c",
		"\xe2\x96\x90", "\xe2\x96\x80", "\xe2\x88\x9e", "\xe2\x88\xa9",
		"\xe2\x89\xa1", "\xe2\x89\xa5", "\xe2\x89\xa4", "\xe2\x8c\xa0",
		"\xe2\x8c\xa1", "\xe2\x89\x88", "\xe2\x88\x99", "\xe2\x88\x9a",
		"\xe2\x81\xbf", "\xe2\x96\xa0"
	};

	return maskChars[rand() % 218];
}

/*
 * char getColorByName(char *string, int fallback)
 *
 * ARGS:
 *
 * char *string - new color setting (white, yellow, black, magenta, blue, green, red)
 *
 * DESCR:
 * Returns an ncurses color by its name.
 *
 */
int getColorByName(char *string) {

	if(string == NULL) {
		return COLOR_BLUE;
	}

	if(strcmp("white", string) == 0) {
		return COLOR_WHITE;
	}

	if(strcmp("yellow", string) == 0) {
		return COLOR_YELLOW;
	}

	if(strcmp("black", string) == 0) {
		return COLOR_BLACK;
	}

	if(strcmp("magenta", string) == 0) {
		return COLOR_MAGENTA;
	}

	if(strcmp("blue", string) == 0) {
		return COLOR_BLUE;
	}

	if(strcmp("green", string) == 0) {
		return COLOR_GREEN;
	}

	if(strcmp("red", string) == 0) {
		return COLOR_RED;
	}

	return COLOR_BLUE;
}
