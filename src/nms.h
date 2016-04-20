#ifndef NMS_H
#define NMS_H 1

#define INIT_NMSARGS { .src = NULL, .return_opts = NULL }

typedef struct {
	char *src;
	char *return_opts;
} NmsArgs;

// Function prototypes

// Display the characters stored in the display queue
char nmsexec(NmsArgs *);

#endif
