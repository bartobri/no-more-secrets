/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

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

// Static settings
static int clearScr          = 0;                     // clearScr flag
static int foregroundColor   = COLOR_BLUE;            // Foreground color setting

// Function prototypes
static void nmstermio_set_terminal(int);

// Initialize terminal window
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
 * Gets and returns the number of rows in the current
 * terminal window.
 */
int nmstermio_get_rows(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_row;
}

/*
 * Gets and returns the number of cols in the current
 * terminal window.
 */
int nmstermio_get_cols(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_col;
}

void nmstermio_move_cursor(int y, int x) {
	CURSOR_MOVE(y, x);
}

void nmstermio_print_string(char *s) {
	printf("%s", s);
}

void nmstermio_refresh(void) {
	fflush(stdout);
}

void nmstermio_clear_input(void) {
	int i;
	
	ioctl(STDIN_FILENO, FIONREAD, &i);

	while (i-- > 0) {
		getchar();
	}
}

char nmstermio_get_char(void) {
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

void nmstermio_show_cursor(void) {
	CURSOR_SHOW();
}

void nmstermio_beep(void) {
	BEEP();
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

/*
 * Returns the row position of the cursor as reported
 * by the terminal program via the ANSI escape code
 */
int nmstermio_get_cursor_row(void) {
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

/*
 * Turns off terminal echo and line buffering when
 * passed an integer value that evaluates to true. It restores the
 * original terminal values when passed an integer value that evaluates
 * to false.
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
