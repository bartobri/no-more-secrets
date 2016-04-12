#include <stdio.h>
#include "nms.h"

int main(void) {
	// Geting input
	int c;
	while ((c = getchar()) != EOF) {
		nmsprintf("%c", c);
	}
	nmsexec();

	return 0;
}
