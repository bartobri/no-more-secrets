#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>

#define SPACE      040                                                                              
#define NEWLINE    012

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
	struct winpos start;
	struct winpos *list_pointer = &start;
	struct winpos *temp;					// Used for free()ing the list
	int termSizeRows = getTermSizeRows();
	int termSizeCols = getTermSizeCols();

	// Geting input
	int c, x = 1, y = 1;
	bool first = true;
	while ((c = getchar()) != EOF) {
		switch (c) {
			case '\n':
				++y;
				x = 1;
				break;
			case ' ':
			case '\t':
			case '\f':
			case '\r':
			case '\v':
				++x;
				break;
			default:
				if (first) {
					start.source = c;
					start.row = y;
					start.col = x;
					start.next = (struct winpos *) 0;
					first = false;
					++x;
				} else {
					// Allocate space for the new struct in our linked list
					// and point *next to it.
					list_pointer->next = malloc(sizeof(struct winpos));

					// Now let's point list_pointer to the next structure
					// and populate it.
					list_pointer = list_pointer->next;
					list_pointer->source = c;
					list_pointer->row = y;
					list_pointer->col = x;
					list_pointer->next = (struct winpos *) 0;

					++x;
				}
				break;
					
		}
	}

	clearTermWindow(termSizeRows, termSizeCols);

	list_pointer = &start;
	while (list_pointer != (struct winpos *) 0) {
		printf("row: %i, ", list_pointer->row);
		printf("col: %i, ", list_pointer->col);
		printf("char: %c\n", list_pointer->source);
		list_pointer = list_pointer->next;
	}

	// Freeing the list. Starting with the second list item because
	// the first one was not created with malloc 
	list_pointer = start.next;
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
