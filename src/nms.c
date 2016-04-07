#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#define SPACE      32
#define NEWLINE    10

int getTermSizeRows(void);
int getTermSizeCols(void);
void clearTermWindow(int, int);

int main(void) {
	struct winpos {
		char source;
		int row;
		int col;
		struct winpos *next;
	};
	struct winpos *list_pointer = NULL;
	struct winpos *start;                   // Always points to start of list
	struct winpos *temp;                    // Used for free()ing the list
	int termSizeRows = getTermSizeRows();
	int termSizeCols = getTermSizeCols();

	// Geting input
	int c, x = 1, y = 1;
	bool first = true;
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

			list_pointer->source = c;
			list_pointer->row = y;
			list_pointer->col = x;
			list_pointer->next = (struct winpos *) 0;

			++x;
		}
	}

	clearTermWindow(termSizeRows, termSizeCols);

	// Printing the list
	list_pointer = start;
	while (list_pointer != (struct winpos *) 0) {
		printf("row: %i, ", list_pointer->row);
		printf("col: %i, ", list_pointer->col);
		printf("char: %c\n", list_pointer->source);
		list_pointer = list_pointer->next;
	}

	// Freeing the list. 
	list_pointer = start;
	while (list_pointer != (struct winpos *) 0) {
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
