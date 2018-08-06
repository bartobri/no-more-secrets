/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GPL License. See LICENSE for more details.
 */

/*
 * The nmstermio modules implements the terminal interface used by the
 * nmseffect module. All terminal IO functionality is defined and
 * implemented here.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

// Support for MinGW on Windows
#ifndef FIONREAD
#include <sys/socket.h>
#endif

// Macros for VT100 codes
#define CLEAR_SCR()          printf("\033[2J")           // Clear Screen
#define CURSOR_HOME()        printf("\033[H")            // Move cursor to home position (0,0)
#define CURSOR_MOVE(y,x)     printf("\033[%i;%iH", y, x) // Move cursor to x,y
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

// Color identifiers
#define COLOR_BLACK   0
#define COLOR_RED     1
#define COLOR_GREEN   2
#define COLOR_YELLOW  3
#define COLOR_BLUE    4
#define COLOR_MAGENTA 5
#define COLOR_CYAN    6
#define COLOR_WHITE   7

// Terminal IO settings
static int clearScr          = 0;                     // clearScr flag
static int foregroundColor   = COLOR_BLUE;            // Foreground color setting

// Function prototypes
static void nmstermio_set_terminal(int);

/*
 * Initialize and configure the terminal for output. This usually means
 * just turning off terminal echo and line buffering. If the clearScr
 * flag is set, it will also save the state of the cursor and screen (so
 * it can be restored) and then clear it.
 */
void nmstermio_init_terminal(void) {

	// Turn off line buffering and echo
	nmstermio_set_terminal(0);
	
	// Save terminal state, clear screen, and home/hide the cursor
	if (clearScr) {
		CURSOR_SAVE();
		SCREEN_SAVE();
		CLEAR_SCR();
		CURSOR_HOME();
		CURSOR_HIDE();
	}
}

/*
 * Restore the state of the terminal to the state prior to executing
 * nmstermio_init_terminal(). This usually means turning on line buffering
 * and echo. If the clearScr flag is set, it will also restore the
 * terminal content and cursor position.
 */
void nmstermio_restore_terminal(void) {
	
	// Restore screen and cursor is clearSrc is set
	if (clearScr) {
		SCREEN_RESTORE();
		CURSOR_SHOW();
		CURSOR_RESTORE();
	}
	
	// Turn on line buffering and echo
	nmstermio_set_terminal(1);
}

/*
 * Get and return the number of rows in the current terminal window.
 */
int nmstermio_get_rows(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_row;
}

/*
 * Get and return the number of cols in the current terminal window.
 */
int nmstermio_get_cols(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_col;
}

/*
 * Move terminal cursor to the given x/y coordinates.
 */
void nmstermio_move_cursor(int y, int x) {
	CURSOR_MOVE(y, x);
}

/*
 * Print the given character string to the terminal output.
 */
void nmstermio_print_string(char *s) {
	printf("%s", s);
}

/*
 * Refresh the screen. Display all pending output that has not been
 * displayed yet.
 */
void nmstermio_refresh(void) {
	fflush(stdout);
}

/*
 * Clear all input pending in STDIN. This is used prior to getting user
 * input to clear the input queue in case of any prior errant keystrokes.
 */
void nmstermio_clear_input(void) {
	int i;
	
	ioctl(STDIN_FILENO, FIONREAD, &i);

	while (i-- > 0) {
		getchar();
	}
}

/*
 * Get a single character of input from the user. Given that line buffering
 * is turned off, we must constantly loop until we get a character other
 * than EOF.
 */
char nmstermio_get_char(void) {
	struct timespec ts;
	int t = 50;
	int c;
	
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;

	while ((c = getchar()) == EOF) {
		nanosleep(&ts, NULL);
	}
	
	return (char)c;
}

/*
 * Print the character string that represents the decrypted data. We
 * apply the bold and color attributes (if colorOn flag is set) to this
 * string.
 */
void nmstermio_print_reveal_string(char *s, int colorOn) {
	
	// Set bold and foreground color
	BOLD();
	if (colorOn) {
		FOREGROUND_COLOR(foregroundColor);
	}
	
	// print string
	printf("%s", s);
	
	// Unset bold and foreground color
	CLEAR_ATTR();
}

/*
 * Display the cursor that have been turned off by nmstermio_init_terminal().
 */
void nmstermio_show_cursor(void) {
	CURSOR_SHOW();
}

/*
 * Make the terminal beep. Used when the returnOpts setting is set in the
 * nmseffect module.
 */
void nmstermio_beep(void) {
	BEEP();
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
 * Get and returns the current row position of the cursor. This involves
 * sending the appropriate ANSI escape code to the terminal and
 * reading/parsing its response.
 */
int nmstermio_get_cursor_row(void) {
	int i, r = 0;
	int row = 0;
	char buf[10];
	char *cmd = "\033[6n";

	memset(buf, 0, sizeof(buf));

	write(STDOUT_FILENO, cmd, strlen(cmd));

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

/*
 * Turn off terminal echo and line buffering when passed an integer value
 * that evaluates to true. Restore the original terminal values when passed
 * an integer value that evaluates to false.
 */
static void nmstermio_set_terminal(int s) {
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
