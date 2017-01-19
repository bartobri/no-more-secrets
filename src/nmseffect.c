/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "nmseffect.h"
#include "nmstermio.h"

// Program settings
#define TYPE_EFFECT_SPEED    4     // miliseconds per char
#define JUMBLE_SECONDS       2     // number of seconds for jumble effect
#define JUMBLE_LOOP_SPEED    35    // miliseconds between each jumble
#define REVEAL_LOOP_SPEED    50    // miliseconds between each reveal loop
#define MASK_CHAR_COUNT      218   // Total characters in maskCharTable[] array.

// Character attribute structure, linked list. Keeps track of every
// character's attributes required for rendering and decryption effect.
struct charAttr {
	char *source;
	char *mask;
	int width;
	int is_space;
	int time;
	struct charAttr *next;
};

// Static function prototypes
static void nmseffect_sleep(int);

// NMS settings
static char *returnOpts     = NULL;         // Return option setting
static int autoDecrypt      = 0;            // Auto-decrypt flag
static int colorOn          = 1;            // Terminal color flag
static int inputPositionX   = -1;           // X coordinate for input position
static int inputPositionY   = -1;           // Y coordinate for input position

// Character table representing the character set know as CP437 used by
// the original IBM PC - https://en.wikipedia.org/wiki/Code_page_437
static char *maskCharTable[] = {
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

/*
 * nmseffect_exec() - This function is passed a pointer to a character string
 * and displays the contents of the string in a way that mimicks the
 * "decrypting text" effect in the 1992 movie Sneakers. It returns the
 * last character pressed by the user.
 */
char nmseffect_exec(char *string) {
	struct charAttr *list_pointer = NULL;
	struct charAttr *list_head    = NULL;
	struct charAttr *list_temp    = NULL;
	int i, revealed = 0;
	int maxRows, maxCols, curRow, curCol, origRow = 0, origCol = 0;
	char ret = 0;

	// Error if we have an empty string.
	if (string == NULL || string[0] == '\0') {
		fprintf(stderr, "Error. Empty string.\n");
		return 0;
	}
	
	// Reassociate STDIN to the terminal is needed
	if (!isatty(STDIN_FILENO) && !freopen ("/dev/tty", "r", stdin)) {
		fprintf(stderr, "Error. Can't associate STDIN with terminal.\n");
		return 0;
	}
	
	// Needed for UTF-8 support
	setlocale(LC_ALL, "");
	
	// Initialize terminal
	nmstermio_init_terminal();
	
	if (!nmstermio_get_clearscr()) {
		// Get current row position
		origRow = nmstermio_get_cursor_row();
		
		// nms_get_cursor_row() may display output in some terminals. So
		// we need to reposition the cursor to the start of the row.
		nmstermio_move_cursor(origRow, 0);
	}

	// Get terminal window rows/cols
	maxRows = nmstermio_get_rows();
	maxCols = nmstermio_get_cols();

	// Seed my random number generator with the current time
	srand(time(NULL));

	// Assign current row/col positions
	curRow = origRow;
	curCol = origCol;

	// Processing input
	for (i = 0; string[i] != '\0'; ++i) {
		
		// Don't go beyond maxRows
		if (curRow - origRow >= maxRows - 1) {
			break;
		}

		// Allocate memory for next list link
		if (list_pointer == NULL) {
			list_pointer = malloc(sizeof(struct charAttr));
			list_head = list_pointer;
		} else {
			list_pointer->next = malloc(sizeof(struct charAttr));
			list_pointer = list_pointer->next;
		}

		// Get character's byte-length and store character.
		if (mblen(&string[i], 4) > 0) {
			list_pointer->source = malloc(mblen(&string[i], 4) + 1);
			strncpy(list_pointer->source, &string[i], mblen(&string[i], 4));
			list_pointer->source[mblen(&string[i], 4)] = '\0';
			i += (mblen(&string[i], 4) - 1);
		} else {
			fprintf(stderr, "Unknown character encountered. Quitting.\n");
			nmstermio_restore_terminal();
			return 0;
		}

		// Set flag if we have a whitespace character
		if (strlen(list_pointer->source) == 1 && isspace(list_pointer->source[0]))
			list_pointer->is_space = 1;
		else
			list_pointer->is_space = 0;

		// Set initial mask chharacter
		list_pointer->mask = maskCharTable[rand() % MASK_CHAR_COUNT];

		// Set reveal time
		list_pointer->time = rand() % 5000;

		// Set character column width
		wchar_t widec[sizeof(list_pointer->source)] = {};
		mbstowcs(widec, list_pointer->source, sizeof(list_pointer->source));
		list_pointer->width = wcwidth(*widec);
		
		// Set next node to null
		list_pointer->next = NULL;

		// Track row count
		if (string[i] == '\n' || (curCol += list_pointer->width) > maxCols) {
			curCol = 0;
			curRow++;
			if (curRow == maxRows + 1 && origRow > 0) {
				origRow--;
				curRow--;
			}
		}
	}
	
	// Print mask characters with 'type effect'
	for (list_pointer = list_head; list_pointer != NULL; list_pointer = list_pointer->next) {

		// Print mask character (or space)
		if (list_pointer->is_space) {
			nmstermio_print_string(list_pointer->source);
			continue;
		}
		
		// print mask character
		nmstermio_print_string(list_pointer->mask);
		if (list_pointer->width == 2) {
			nmstermio_print_string(maskCharTable[rand() % MASK_CHAR_COUNT]);
		}
		
		// flush output and sleep
		nmstermio_refresh();
		nmseffect_sleep(TYPE_EFFECT_SPEED);
	}

	// Flush any input up to this point
	nmstermio_clear_input();

	// If autoDecrypt flag is set, we sleep. Otherwise require user to
	// press a key to continue.
	if (autoDecrypt)
		sleep(1);
	else
		nmstermio_get_char();

	// Jumble loop
	for (i = 0; i < (JUMBLE_SECONDS * 1000) / JUMBLE_LOOP_SPEED; ++i) {
		
		// Move cursor to start position
		nmstermio_move_cursor(origRow, origCol);
		
		// Print new mask for all characters
		for (list_pointer = list_head; list_pointer != NULL; list_pointer = list_pointer->next) {
	
			// Print mask character (or space)
			if (list_pointer->is_space) {
				nmstermio_print_string(list_pointer->source);
				continue;
			}
			
			// print new mask character
			nmstermio_print_string(maskCharTable[rand() % MASK_CHAR_COUNT]);
			if (list_pointer->width == 2) {
				nmstermio_print_string(maskCharTable[rand() % MASK_CHAR_COUNT]);
			}
		}
		
		// flush output and sleep
		nmstermio_refresh();
		nmseffect_sleep(JUMBLE_LOOP_SPEED);
	}

	// Reveal loop
	while (!revealed) {
		
		// Move cursor to start position
		nmstermio_move_cursor(origRow, origCol);
		
		// Set revealed flag
		revealed = 1;
		
		for (list_pointer = list_head; list_pointer != NULL; list_pointer = list_pointer->next) {
	
			// Print mask character (or space)
			if (list_pointer->is_space) {
				nmstermio_print_string(list_pointer->source);
				continue;
			}
			
			// If we still have time before the char is revealed, display the mask
			if (list_pointer->time > 0) {
				
				// Change the mask randomly
				if (list_pointer->time < 500) {
					if (rand() % 3 == 0) {
						list_pointer->mask = maskCharTable[rand() % MASK_CHAR_COUNT];
					}
				} else {
					if (rand() % 10 == 0) {
						list_pointer->mask = maskCharTable[rand() % MASK_CHAR_COUNT];
					}
				}
				
				// Print mask
				nmstermio_print_string(list_pointer->mask);
				
				// Decrement reveal time
				list_pointer->time -= REVEAL_LOOP_SPEED;
				
				// Unset revealed flag
				revealed = 0;
			} else {
				
				// print source character
				nmstermio_print_reveal_string(list_pointer->source, colorOn);
			}
		}

		// flush output and sleep
		nmstermio_refresh();
		nmseffect_sleep(REVEAL_LOOP_SPEED);
	}

	// Flush any input up to this point
	nmstermio_clear_input();

	// Check if user must select from a set of options
	if (returnOpts != NULL && strlen(returnOpts) > 0) {
		
		// Position cursor if necessary
		if (inputPositionY >= 0 && inputPositionX >= 0) {
			nmstermio_move_cursor(inputPositionY, inputPositionX);
		}
		
		nmstermio_show_cursor();
		
		// Get and validate user selection
		while (strchr(returnOpts, ret = nmstermio_get_char()) == NULL) {
			nmstermio_beep();
		}

	}
	
	// User must press a key to continue when clearSrc is set
	// without returnOpts
	else if (nmstermio_get_clearscr()) {
		nmstermio_get_char();
	}
	
	// Restore terminal
	nmstermio_restore_terminal();

	// Freeing the list. 
	list_pointer = list_head;
	while (list_pointer != NULL) {
		list_temp = list_pointer;
		list_pointer = list_pointer->next;
		free(list_temp->source);
		free(list_temp);
	}

	return ret;
}

/*
 * nmseffect_set_foreground_color() sets the foreground color of the unencrypted
 * characters as they are revealed to the color indicated by the 'color'
 * argument. Valid arguments are "white", "yellow", "magenta", "blue",
 * "green", "red", and "cyan". This function will default to blue if
 * passed an invalid color. No value is returned.
 */
void nmseffect_set_foregroundcolor(char *color) {
	nmstermio_set_foregroundcolor(color);
}

/*
 * nmseffect_set_returnopts() takes a character sting and copies it to the
 * returnOpts setting used by nms_exec().
 */
void nmseffect_set_returnopts(char *opts) {
	returnOpts = realloc(returnOpts, strlen(opts) + 1);
	strcpy(returnOpts, opts);
}

/*
 * nmseffect_set_autodecrypt() sets the autoDecrypt flag according to the
 * true/false value of the 'setting' argument.
 */
void nmseffect_set_autodecrypt(int setting) {
	if (setting)
		autoDecrypt = 1;
	else
		autoDecrypt = 0;
}

/*
 * nmseffect_set_clearscr() sets the clearScr flag according to the
 * true/false value of the 'setting' argument.
 */
void nmseffect_set_clearscr(int s) {
	nmstermio_set_clearscr(s);
}

/*
 * nmseffect_set_color() sets the colorOn flag according to the
 * true/false value of the 'setting' argument.
 */
void nmseffect_set_color(int setting) {
	if (setting)
		colorOn = 1;
	else
		colorOn = 0;
}

/*
 * nmseffect_set_input_position() sets the desired coordinate of the cursor in
 * the terminal when accepting user input after nms_exec() reveals the
 * unencrypted characters.
 */
void nmseffect_set_input_position(int x, int y) {
	if (x >= 0 && y >= 0) {
		inputPositionX = x;
		inputPositionY = y;
	}
}

/*
 * nmseffect_sleep() sleeps for the number of miliseconds indicated by 't'.
 */
static void nmseffect_sleep(int t) {
	struct timespec ts;
	
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	
	nanosleep(&ts, NULL);
}
