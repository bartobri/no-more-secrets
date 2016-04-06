#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define SPACE      040                                                                              
#define NEWLINE    012

int getTermSizeRows(void);
int getTermSizeCols(void);
void clearTermWindow(int, int);

int main(void) {
	int termSizeRows = getTermSizeRows();
	int termSizeCols = getTermSizeCols();
	char *inputBuffer;

	clearTermWindow(termSizeRows, termSizeCols);

	// Geting input
	int c, i = 0;
	while ((c = getchar()) != EOF) {
		inputBuffer = realloc(inputBuffer, sizeof(char) * (i + 2));
		inputBuffer[i] = c;
		++i;
	}
	inputBuffer[i] = '\0';

	printf("%s", inputBuffer);

	free(inputBuffer);

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
