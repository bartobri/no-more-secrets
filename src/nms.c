#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define SPACE      32
#define NEWLINE    10

#define KNRM       "\x1B[0m"
#define KMAG       "\x1B[35m"
#define KCYN       "\x1B[36m"

int getTermSizeRows(void);
int getTermSizeCols(void);
void clearTermWindow(int, int);
char getMaskChar(void);

int main(void) {
	struct winpos {
		char source;
		char mask;
		int row;
		int col;
		int s1_time;
		int s2_time;
		struct winpos *next;
	};
	struct winpos *list_pointer = NULL;
	struct winpos *start;                   // Always points to start of list
	struct winpos *temp;                    // Used for free()ing the list
	int termSizeRows = getTermSizeRows();
	int termSizeCols = getTermSizeCols();
	int c, x = 1, y = 1;
	int r_time, r_time_l, r_time_s;
	int ms, ls;
	bool first = true;

	// Seed my random number generator with the current time
	srand(time(NULL));

	// TODO: what is the data piped is greater than the size of the terminal window?

	// Geting input
	while ((c = getchar()) != EOF) {
		if (c == NEWLINE) {
			++y;
			x = 1;
		} else if (isspace(c)) {
			++x;
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

			++x;
		}
	}

	clearTermWindow(termSizeRows, termSizeCols);

	// TODO: Data should initially 'type' itself on to the screen

	// TODO: pause with getchar() - something about the input stream being redirected
	// to a file is causing getchar() to immediately return here.

	// Jumble loop
	ms = 35;           // miliseconds, used for usleep()
	ls = 2;            // loop seconds, number of seconds to loop
	x = 0;
	while (x < (ls * 1000) / ms) {
		list_pointer = start;
		while (list_pointer != NULL) {
			printf("\033[%i;%iH%c", list_pointer->row, list_pointer->col, list_pointer->mask);
			list_pointer->mask = getMaskChar();
			list_pointer = list_pointer->next;
		}
		fflush(stdout);
		usleep(ms * 1000);
		++x;
	}


	// Reveal loop
	ms = 100;                     // miliseconds to sleep (must always evenly divisible by 5)
	int s1_remask_time = 500;     // time, in milliseconds, we change the mask for stage 1
	bool loop = true;
	while (loop) {
		loop = false;
		list_pointer = start;
		while (list_pointer != NULL) {
			if (list_pointer->s1_time > 0) {
				loop = true;
				list_pointer->s1_time -= ms;
				if (list_pointer->s1_time % s1_remask_time == 0) {
					list_pointer->mask = getMaskChar();
				}
			} else if (list_pointer->s2_time > 0) {
				loop = true;
				list_pointer->s2_time -= ms;
				list_pointer->mask = getMaskChar();
			} else {
				list_pointer->mask = list_pointer->source;
				printf(KCYN);
			}
			printf("\033[%i;%iH%c", list_pointer->row, list_pointer->col, list_pointer->mask);
			printf(KNRM);
			list_pointer = list_pointer->next;
		}
		fflush(stdout);
		usleep(ms * 1000);
	}

	printf("\n");

	// Freeing the list. 
	list_pointer = start;
	while (list_pointer != NULL) {
		temp = list_pointer;
		list_pointer = list_pointer->next;
		free(temp);
	}
	

	return 0;
}

int getTermSizeRows(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	return w.ws_row;
}

int getTermSizeCols(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

	return w.ws_col;
}

void clearTermWindow(int pRows, int pCols) {
	int row, col;

	// Clearing window
	for (row = 1; row <= pRows; ++row) {
		for (col = 1; col <= pCols; ++col)
			printf("\033[%i;%iH%c", row, col, SPACE);
		printf("\033[%i;%iH%c", row, col, NEWLINE);
	}

	// Position cursor at the top
	printf("\033[%i;%iH", 1, 1);
}

char getMaskChar(void) {
	char *maskChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	                  "abcdefghijklmnopqrstuvwxyz"
	                  "1234567890";

	return maskChars[rand() % strlen(maskChars)];
}
