#ifndef NMS_H
#define NMS_H 1

#include <stdbool.h>

// Default arguments for nms_exec()
#define INIT_NMSARGS { NULL, NULL, -1, -1, false }

// Argument structure for nms_exec()
typedef struct {
	char *src;
	char *return_opts;
	int input_cursor_x;
	int input_cursor_y;
	bool show_cursor;
} NmsArgs;

// Display the characters stored in the display queue
char nms_exec(NmsArgs *);

#endif
