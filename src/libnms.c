/*
 * Copyright (c) 2016 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */
 
/* 
 * DESCRIPTION
 * 
 * The libnms library encapsulates the functuionality required to produce the
 * famous "decrypting text" sceen effect from the 1992 hacker movie "Sneakers".
 * For more, go to: https://github.com/bartobri/libnms
 */

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include "libnms.h"

// Color identifiers
#define COLOR_BLACK   0
#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_MAGENTA 5
#define COLOR_CYAN    6
#define COLOR_WHITE   7

// Macros for VT100 codes
#define CLEAR_SCR()          printf("\033[2J")           // Clear Screen
#define CURSOR_HOME()        printf("\033[H")            // Move cursor to home position (0,0)
#define CURSOR_MOVE(x,y)     printf("\033[%i;%iH", x, y) // Move cursor to x,y
#define BEEP()               printf("\a");               // terminal bell
#define BOLD()               printf("\033[1m")           // Cursor bold
#define FOREGROUND_COLOR(x)  printf("\033[3%im", x)      // Set foreground color
#define CLEAR_ATTR()         printf("\033[0m")           // Clear bold/color attributes
#define SCREEN_SAVE()        printf("\033[?47h")         // Save screen display
#define SCREEN_RESTORE()     printf("\033[?47l")         // Restore screen to previously saved state
#define CURSOR_SAVE()        printf("\033[s")            // Save cursor position
#define CURSOR_RESTORE()     printf("\033[u")            // Restore cursor position
#define CURSOR_HIDE()        printf("\033[?25l")         // Hide cursor
#define CURSOR_SHOW()        printf("\033[?25h")         // Unhide cursor

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
static void nms_sleep(int);
static int  nms_term_rows(void);
static int  nms_term_cols(void);
static void nms_set_terminal(int s);
static void nms_clear_input(void);
static char nms_get_char(void);
static int  nms_get_cursor_row(void);

// NMS settings
static int foregroundColor  = COLOR_BLUE;   // Foreground color setting
static char *returnOpts     = NULL;         // Return option setting
static int autoDecrypt      = 0;            // Auto-decrypt flag
static int clearScr         = 0;            // clearScr flag
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
 * nms_exec() - This function is passed a pointer to a character string
 * and displays the contents of the string in a way that mimicks the
 * "decrypting text" effect in the 1992 movie Sneakers. It returns the
 * last character pressed by the user.
 */
char nms_exec(char *string) {
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
	
	// Turn off terminal echo and line buffering
	nms_set_terminal(0);

	// Needed for UTF-8 support
	setlocale(LC_ALL, "");

	// Get terminal window rows/cols
	maxRows = nms_term_rows();
	maxCols = nms_term_cols();

	// Seed my random number generator with the current time
	srand(time(NULL));
	
	// Get cursor position if we are not clearing the screen
	if (!clearScr) {
		origRow = nms_get_cursor_row();
		
		// nms_get_cursor_row() may display output in some terminals. So
		// we need to reposition the cursor to the start of the row.
		CURSOR_MOVE(origRow, origCol);
	}

	// Assign current row/col positions
	curRow = origRow;
	curCol = origCol;

	// Geting input
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
			nms_set_terminal(1);
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
	
	// Save terminal state, clear screen, and home/hide the cursor
	if (clearScr) {
		CURSOR_SAVE();
		SCREEN_SAVE();
		CLEAR_SCR();
		CURSOR_HOME();
		CURSOR_HIDE();
	}
	
	// Print mask characters with 'type effect'
	for (list_pointer = list_head; list_pointer != NULL; list_pointer = list_pointer->next) {

		// Print mask character (or space)
		if (list_pointer->is_space) {
			printf("%s", list_pointer->source);
			continue;
		}
		
		// print mask character
		printf("%s", list_pointer->mask);
		if (list_pointer->width == 2) {
			printf("%s", maskCharTable[rand() % MASK_CHAR_COUNT]);
		}
		
		// flush output and sleep
		fflush(stdout);
		nms_sleep(TYPE_EFFECT_SPEED);
	}

	// Flush any input up to this point
	nms_clear_input();

	// If autoDecrypt flag is set, we sleep. Otherwise require user to
	// press a key to continue.
	if (autoDecrypt)
		sleep(1);
	else
		nms_get_char();

	// Jumble loop
	for (i = 0; i < (JUMBLE_SECONDS * 1000) / JUMBLE_LOOP_SPEED; ++i) {
		
		// Move cursor to home position
		if (clearScr) {
			CURSOR_HOME();
		} else {
			CURSOR_MOVE(origRow, origCol);
		}
		
		// Print new mask for all characters
		for (list_pointer = list_head; list_pointer != NULL; list_pointer = list_pointer->next) {
	
			// Print mask character (or space)
			if (list_pointer->is_space) {
				printf("%s", list_pointer->source);
				continue;
			}
			
			// print new mask character
			printf("%s", maskCharTable[rand() % MASK_CHAR_COUNT]);
			if (list_pointer->width == 2) {
				printf("%s", maskCharTable[rand() % MASK_CHAR_COUNT]);
			}
		}
		
		// flush output and sleep
		fflush(stdout);
		nms_sleep(JUMBLE_LOOP_SPEED);
	}

	// Reveal loop
	while (!revealed) {
		
		// Move cursor to home position
		if (clearScr) {
			CURSOR_HOME();
		} else {
			CURSOR_MOVE(origRow, origCol);
		}
		
		// Set revealed flag
		revealed = 1;
		
		for (list_pointer = list_head; list_pointer != NULL; list_pointer = list_pointer->next) {
	
			// Print mask character (or space)
			if (list_pointer->is_space) {
				printf("%s", list_pointer->source);
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
				printf("%s", list_pointer->mask);
				
				// Decrement reveal time
				list_pointer->time -= REVEAL_LOOP_SPEED;
				
				// Unset revealed flag
				revealed = 0;
			} else {
				
				// Set bold and foreground color for character reveal
				BOLD();
				if (colorOn) {
					FOREGROUND_COLOR(foregroundColor);
				}
				
				// print source character
				printf("%s", list_pointer->source);
				
				// Unset foreground color
				CLEAR_ATTR();
			}
		}

		// flush output and sleep
		fflush(stdout);
		nms_sleep(REVEAL_LOOP_SPEED);
	}

	// Flush any input up to this point
	nms_clear_input();

	if (clearScr) {

		// Position cursor
		if (inputPositionY >= 0 && inputPositionX >= 0) {
			CURSOR_MOVE(inputPositionY, inputPositionX);
			CURSOR_SHOW();
		}
	
		// User must press a key to continue
		ret = nms_get_char();
		if (returnOpts != NULL && strlen(returnOpts) > 0) {
			while (strchr(returnOpts, ret) == NULL) {
				BEEP();
				ret = nms_get_char();
			}
		}
	
		// Restore screen and cursor
		SCREEN_RESTORE();
		CURSOR_SHOW();
		CURSOR_RESTORE();
	}
	
	// Turn on terminal echo and line buffering
	nms_set_terminal(1);

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
 * nms_set_foreground_color() sets the foreground color of the unencrypted
 * characters as they are revealed to the color indicated by the 'color'
 * argument. Valid arguments are "white", "yellow", "magenta", "blue",
 * "green", "red", and "cyan". This function will default to blue if
 * passed an invalid color. No value is returned.
 */
void nms_set_foreground_color(char *color) {

	if(strcmp("white", color) == 0)
		foregroundColor =  COLOR_WHITE;
	else if(strcmp("yellow", color) == 0)
		foregroundColor = COLOR_YELLOW;
	else if(strcmp("black", color) == 0)
		foregroundColor = COLOR_BLACK;
	else if(strcmp("magenta", color) == 0)
		foregroundColor = COLOR_MAGENTA;
	else if(strcmp("blue", color) == 0)
		foregroundColor = COLOR_BLUE;
	else if(strcmp("green", color) == 0)
		foregroundColor = COLOR_GREEN;
	else if(strcmp("red", color) == 0)
		foregroundColor = COLOR_RED;
	else if(strcmp("cyan", color) == 0)
		foregroundColor = COLOR_CYAN;
	else
		foregroundColor = COLOR_BLUE;
}

/*
 * nms_set_return_opts() takes a character sting and copies it to the
 * returnOpts setting used by nms_exec().
 */
void nms_set_return_opts(char *opts) {
	returnOpts = realloc(returnOpts, strlen(opts) + 1);
	strcpy(returnOpts, opts);
}

/*
 * nms_set_auto_decrypt() sets the autoDecrypt flag according to the
 * true/false value of the 'setting' argument.
 */
void nms_set_auto_decrypt(int setting) {
	if (setting)
		autoDecrypt = 1;
	else
		autoDecrypt = 0;
}

/*
 * nms_set_clear_scr() sets the clearScr flag according to the
 * true/false value of the 'setting' argument.
 */
void nms_set_clear_scr(int setting) {
	if (setting)
		clearScr = 1;
	else
		clearScr = 0;
}

/*
 * nms_set_color() sets the colorOn flag according to the
 * true/false value of the 'setting' argument.
 */
void nms_use_color(int setting) {
	if (setting)
		colorOn = 1;
	else
		colorOn = 0;
}

/*
 * nms_set_input_position() sets the desired coordinate of the cursor in
 * the terminal when accepting user input after nms_exec() reveals the
 * unencrypted characters.
 */
void nms_set_input_position(int x, int y) {
	if (x >= 0 && y >= 0) {
		inputPositionX = x;
		inputPositionY = y;
	}
}

/*
 * nms_sleep() sleeps for the number of miliseconds indicated by 't'.
 */
static void nms_sleep(int t) {
	struct timespec ts;
	
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	
	nanosleep(&ts, NULL);
}

/*
 * nms_term_rows() gets and returns the number of rows in the current
 * terminal window.
 */
static int nms_term_rows(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_row;
}

/*
 * nms_term_cols() gets and returns the number of cols in the current
 * terminal window.
 */
static int nms_term_cols(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_col;
}

/*
 * nms_set_terminal() turns off terminal echo and line buffering when
 * passed an integer value that evaluates to true. It restores the
 * original terminal values when passed an integer value that evaluates
 * to false.
 */
static void nms_set_terminal(int s) {
	struct termios tp;
	static struct termios save;
	static int state = 1;
	
	if (s == 0) {
		if (tcgetattr(STDIN_FILENO, &tp) == -1) {
			return;
		}

		save = tp;
		
		tp.c_lflag &=(~ICANON & ~ECHO);
		
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1) {
			return;
		}
	} else {
		if (state == 0 && tcsetattr(STDIN_FILENO, TCSANOW, &save) == -1)
			return;
	}
	
	state = s;
}

/*
 * nms_clear_input() clears the input buffer of all characters up to
 * the EOF character.
 */
static void nms_clear_input(void) {
	int i;
	
	ioctl(STDIN_FILENO, FIONREAD, &i);

	while (i > 0) {
		getchar();
		--i;
	}
}

/*
 * nms_get_char() returns the next character in the input buffer. In the
 * case of an EOF character, it blocks until the user presses a key.
 */
static char nms_get_char(void) {
	struct timespec ts;
	int t = 50;
	char c;
	
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;

	while ((c = getchar()) == EOF) {
		nanosleep(&ts, NULL);
	}
	
	return c;
}

/*
 * nms_get_cursor_row() returns the row position of the cursor as reported
 * by the terminal program via the ANSI escape code
 */
static int nms_get_cursor_row(void) {
	int i, r = 0;
	int row = 0;
	char buf[10];
	char *cmd = "\033[6n";

	memset(buf, 0, sizeof(buf));

	write(STDOUT_FILENO, cmd, sizeof(cmd));

	r = read(STDIN_FILENO, buf, sizeof(buf));

	for (i = 0; i < r; ++i) {
		if (buf[i] == 27 || buf[i] == '[') {
			continue;
		}

		if (buf[i] >= '0' && buf[i] <= '9') {
			row = (row * 10) + (buf[i] - '0');
		}
		
		if (buf[i] == ';' || buf[i] == 'R' || buf[i] == 0) {
			break;
		}
	}
	
	return row;
}
