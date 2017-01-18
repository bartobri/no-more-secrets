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

// Static settings
static int clearScr = 0;                                 // clearScr flag

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

// Function prototypes
static void nmsterm_set_terminal(int);
static int nmsterm_get_cursor_row(void);

// Initialize terminal window
int nmsterm_init_terminal(int foregroundColor, int c) {
	(void) foregroundColor;
	int origRow = 0;
	
	// Set clearScr flag
	clearScr = c;

	// Turn off line buffering and echo
	nmsterm_set_terminal(0);
	
	// Save terminal state, clear screen, and home/hide the cursor
	if (clearScr) {
		CURSOR_SAVE();
		SCREEN_SAVE();
		CLEAR_SCR();
		CURSOR_HOME();
		CURSOR_HIDE();
	} else {
		
		// Get current row position
		origRow = nmsterm_get_cursor_row();
		
		// nms_get_cursor_row() may display output in some terminals. So
		// we need to reposition the cursor to the start of the row.
		CURSOR_MOVE(origRow, 0);
	}
	
	// Return cursor row position
	return origRow;
}

void nmsterm_restore_terminal(void) {
	
	// Restore screen and cursor is clearSrc is set
	if (clearScr) {
		SCREEN_RESTORE();
		CURSOR_SHOW();
		CURSOR_RESTORE();
	}
	
	// Turn on line buffering and echo
	nmsterm_set_terminal(1);
}

/*
 * nms_term_rows() gets and returns the number of rows in the current
 * terminal window.
 */
int nmsterm_get_rows(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_row;
}

/*
 * nms_term_cols() gets and returns the number of cols in the current
 * terminal window.
 */
int nmsterm_get_cols(void) {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
	return w.ws_col;
}

void nmsterm_move_cursor(int y, int x) {
	CURSOR_MOVE(y, x);
}

void nmsterm_print_string(char *s) {
	printf("%s", s);
}

void nmsterm_refresh(void) {
	fflush(stdout);
}

void nmsterm_clear_input(void) {
	int i;
	
	ioctl(STDIN_FILENO, FIONREAD, &i);

	while (i-- > 0) {
		getchar();
	}
}

char nmsterm_get_char(void) {
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

void nmsterm_print_reveal_string(char *s, int foregroundColor, int colorOn) {
	
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

void nmsterm_show_cursor(void) {
	CURSOR_SHOW();
}

void nmsterm_beep(void) {
	BEEP();
}

int nmsterm_get_clearscr(void) {
	return clearScr;
}

/*
 * nmsterm_set_terminal() turns off terminal echo and line buffering when
 * passed an integer value that evaluates to true. It restores the
 * original terminal values when passed an integer value that evaluates
 * to false.
 */
static void nmsterm_set_terminal(int s) {
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
 * nms_get_cursor_row() returns the row position of the cursor as reported
 * by the terminal program via the ANSI escape code
 */
static int nmsterm_get_cursor_row(void) {
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
