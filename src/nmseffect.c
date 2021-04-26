/*
 * Copyright (c) 2017 Brian Barto
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GPL License. See LICENSE for more details.
 */

/*
 * The nmseffect module is the primary module that drives the effect
 * execution. Most attributes, settings, and code that define the behavior
 * is implemented here.
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
#include "nmscharset.h"

// Speed settings
#define TYPE_EFFECT_SPEED    4     // miliseconds per char
#define JUMBLE_LOOP_SPEED    35    // miliseconds between each jumble
#define REVEAL_TIME_SPEED    50    // miliseconds of reveal time step

// Behavior settings
static int autoDecrypt      = 0;      // Auto-decrypt flag
static int maskBlank        = 0;      // Mask blank spaces
static int colorOn          = 1;      // Terminal color flag
static int jumbleMillis     = 2000;   // number of millis for jumble effect
static int revealLoopSpeed  = 50;     // miliseconds between each reveal loop

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

/*
 * This function applies the data decryption effect to the character
 * string that is provided as an argument. It returns the last character
 * pressed by the user.
 */
char nmseffect_exec(unsigned char *string, int string_len) {
	struct charAttr *list_pointer = NULL;
	struct charAttr *list_head    = NULL;
	struct charAttr *list_temp    = NULL;
	int i, l, revealed = 0;
	int maxRows, maxCols, curRow, curCol, origRow = 0, origCol = 0;
	char ret = 0;

	// Needed for UTF-8 support
	setlocale(LC_ALL, "");

	// Seed my random number generator with the current time
	srand(time(NULL));

	// Initialize terminal
	nmstermio_init_terminal();

	if (!nmstermio_get_clearscr()) {
		// Get current row position
		origRow = nmstermio_get_cursor_row();

		// nmstermio_get_cursor_row() may display output in some terminals. So
		// we need to reposition the cursor to the start of the row, print
		// some blank spaces, and the reposition again.
		nmstermio_move_cursor(origRow, 0);
		nmstermio_print_string("    ");
		nmstermio_move_cursor(origRow, 0);
	}

	// Get terminal window rows/cols
	maxRows = nmstermio_get_rows();
	maxCols = nmstermio_get_cols();

	// Assign current row/col positions
	curRow = origRow;
	curCol = origCol;

	// Processing input
	for (i = 0; i < string_len; ++i) {

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
		l = mblen((char *)&string[i], 4);
		if (l > 0) {
			list_pointer->source = malloc(l + 1);
			memcpy(list_pointer->source, &string[i], l);
			list_pointer->source[l] = '\0';
			i += (l - 1);
		} else {
			fprintf(stderr, "Unknown character encountered. Quitting.\n");
			nmstermio_restore_terminal();
			return 0;
		}

		// Set flag if we have a whitespace character
		if (strlen(list_pointer->source) == 1 && isspace(list_pointer->source[0])) {

			// If flag is enabled, mask blank spaces as well
			if (maskBlank && (list_pointer->source[0] == ' ')) {
				list_pointer->is_space = 0;
			} else {
				list_pointer->is_space = 1;
			}

		} else {
			list_pointer->is_space = 0;
		}

		// Set initial mask chharacter
		list_pointer->mask = nmscharset_get_random();

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
			nmstermio_print_string(nmscharset_get_random());
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
	for (i = 0; i < jumbleMillis / JUMBLE_LOOP_SPEED; ++i) {

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
			nmstermio_print_string(nmscharset_get_random());
			if (list_pointer->width == 2) {
				nmstermio_print_string(nmscharset_get_random());
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
						list_pointer->mask = nmscharset_get_random();
					}
				} else {
					if (rand() % 10 == 0) {
						list_pointer->mask = nmscharset_get_random();
					}
				}

				// Print mask
				nmstermio_print_string(list_pointer->mask);

				// Decrement reveal time
				list_pointer->time -= REVEAL_TIME_SPEED;

				// Unset revealed flag
				revealed = 0;
			} else {

				// print source character
				nmstermio_print_reveal_string(list_pointer->source, colorOn);
			}
		}

		// flush output and sleep
		nmstermio_refresh();
		nmseffect_sleep(revealLoopSpeed);
	}

	nmstermio_clear_input();

	if (nmstermio_get_clearscr()) {
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
 * Pass the 'color' argument to the nmstermio module where it will set the
 * foreground color of the unencrypted characters as they are
 * revealed. Valid arguments are "white", "yellow", "magenta", "blue",
 * "green", "red", and "cyan".
 */
void nmseffect_set_foregroundcolor(char *color) {
	nmstermio_set_foregroundcolor(color);
}

/*
 * Set jumbleMillis according to the value parsed from the
 * 'time' argument, with a minimum of 0. This adjusts the duration
 * of the jumbling. Also adjusts the revealLoopSpeed to a 1:40 ratio.
 */
void nmseffect_set_jumbletime(char *time) {
  float millis = atoi(time);
  if (millis < 0)
    jumbleMillis = 0;
  else
    jumbleMillis = millis;
  revealLoopSpeed = jumbleMillis/40;
}

/*
 * Set the autoDecrypt flag according to the true/false value of the
 * 'setting' argument. When set to true, nmseffect_exec() will not
 * require a key press to start the decryption effect.
 */
void nmseffect_set_autodecrypt(int setting) {
	if (setting)
		autoDecrypt = 1;
	else
		autoDecrypt = 0;
}

/*
 * Set the maskBlank flag according to the true/false value of the
 * 'setting' argument. When set to true, blank spaces characters
 * will be masked as well.
 */
void nmseffect_set_maskblank(int setting) {
    if (setting)
        maskBlank = 1;
    else
        maskBlank = 0;
}

/*
 * Pass the 'setting' argument to the nmstermio module where it will set
 * the clearScr flag according to the true/false value. When set to true,
 * nmseffect_exec() will clear the screen before displaying any characters.
 */
void nmseffect_set_clearscr(int setting) {
	nmstermio_set_clearscr(setting);
}

/*
 * Set the 'colorOn' flag according to the true/false value of the 'setting'
 * argument. This flag tells nmseffect_exec() to use colors in it's output.
 * This is true by default, and meant to be used only for terminals that
 * do not support color. Though, compiling with ncurses support is perhaps
 * a better option, as it will detect color capabilities automatically.
 */
void nmseffect_set_color(int setting) {
	if (setting)
		colorOn = 1;
	else
		colorOn = 0;
}

/*
 * Sleep for the number of milliseconds indicated by argument 't'.
 */
static void nmseffect_sleep(int t) {
	struct timespec ts;

	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;

	nanosleep(&ts, NULL);
}
