#ifndef NMS_H
#define NMS_H 1

#include <stdbool.h>

#define INIT_NMSARGS { NULL, NULL, -1, -1, false }

typedef struct {
	char *src;
	char *return_opts;
	int input_cursor_x;
	int input_cursor_y;
	bool show_cursor;
} NmsArgs;

// Function prototypes

// Display the characters stored in the display queue
char nms_exec(NmsArgs *);

#endif
