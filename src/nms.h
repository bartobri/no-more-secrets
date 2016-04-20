#ifndef NMS_H
#define NMS_H 1

#define INIT_NMSARGS { .src = NULL, .return_opts = NULL, .input_cursor_x = -1, .input_cursor_y = -1 }

typedef struct {
	char *src;
	char *return_opts;
	int input_cursor_x;
	int input_cursor_y;
} NmsArgs;

// Function prototypes

// Display the characters stored in the display queue
char nms_exec(NmsArgs *);

#endif
