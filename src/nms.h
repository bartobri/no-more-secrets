/*
 * Copyright (c) 2017 Brian Barto
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the MIT License. See LICENSE for more details.
 */

#ifndef NMS_H
#define NMS_H 1

// Function prototypes
char nms_exec(char *);
void nms_set_foreground_color(char *);
void nms_set_return_opts(char *);
void nms_set_auto_decrypt(int);
void nms_set_clear_scr(int);
void nms_use_color(int);
void nms_set_input_position(int, int);

#endif
