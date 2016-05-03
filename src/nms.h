#ifndef NMS_H
#define NMS_H 1

#include <stddef.h>
#include <stdbool.h>
#include <ncurses.h>

// Default arguments for nms_exec()
#define INIT_NMSARGS { NULL, NULL, -1, -1, false, false , COLOR_BLUE }

// Argument structure for nms_exec()
typedef struct {
	char *src;
	char *return_opts;
	int input_cursor_x;
	int input_cursor_y;
	bool show_cursor;
	bool auto_decrypt;
	int foreground_color;
} NmsArgs;

// Display the characters stored in the display queue
char nms_exec(NmsArgs *);

int  getColorByName(char *string, int fallback);

#endif
