No More Secrets
===============

"No More Secrets" is the name I've lovingly given to the infamous "decrypting text effect" seen on
screen in the 1992 hacker movie "Sneakers". If you are unfamiliar with the effect, it can be seen
at 0:35 in [this youtube video](https://www.youtube.com/watch?v=F5bAa6gFvLs).

This project seeks to provide tools to recreate this effect in your projects.

Here is a standalone executable (provided in this project) that uses the provided tools to perform this
effect on piped data:

![Screenshot](http://i.imgur.com/ezF3xkN.gif)

Here is another example program called "sneakers" (type `make sneakers` when installing.) that
utilizes these tools to recreate, almost identically, what we see in the above movie clip.

![Screenshot](http://www.tackboard.world/no_more_secrets.gif)

What's Provided
---------------

Two tools are provided for you to recreate this effect:

1. A standalone executable file named `nms` (shown above). This file accepts data from a shell pipe and displays it in a manner that is nearly identical to the effect we see in the movie.

2. A module and header file, written in C, that can be included in programs and used to recreate this effect in a custonized way. In fact, the standalone executable `nms` is really just an example that shows how to use this module on piped data.

Installing the Standalone Executable
------------------------------------
```
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
```

This will create an executable file named `nms` and place it in the `no-more-secrets/bin/` directory.
You may manually copy it in to your preferred bin directory.

Using the Standalone Executable
-------------------------------

`nms` accepts data from a shell pipe. Simply pipe some data to it and enjoy the magic!
```
ls -l | bin/nms
```
Once the "encrypted" data is displayed, the program will pause until you press a key. Then the
decryption effect will start. After that is completed, it will again pause until
you press a key, at which point the program will terminate.

Using the Modules in Your Program
---------------------------------

##### Synopsis

File: synopsis.c
```
#include "nms.h"

int main() {
    NmsArgs args = INIT_NMSARGS;

    args.src = "This is a simple test";

    nms_exec(&args);

    return 0;
}

```
Compile synopsis.c (must include `nms.c` and `-lncurses`):
```
gcc nms.c synopsis.c -o synopsis -lncurses
```

##### The Details

You will need to copy two files to your project:

```
nms.h
nms.c
```
Include `nms.h` in your program file:

```
#include <nms.h>
```
Add nms.c to your source file list when compiling:
```
gcc nms.c myprog.c -o myprog
```
You only need to call one function from inside your program to create this effect. It is defined as
such:
```
char nms_exec(NmsArgs *);
```
This function only takes one argument: a pointer to a structure containing 5 members. One of these
members is a character pointer to a string on which you want to perform this effect. The other
members are explained below.

First, declare the structure like so (substitute 'args' with whatever name you desire):
```
NmsArgs args = INIT_NMSARGS;
```
The defined name INIT_NMSARGS represents a default set of values for all 5 members. This way, you can
ignore the members you don't want to use and only program for those that you do. Below is the
definition for the NmsArgs structure:
```
typedef struct {
    char *src;
    char *return_opts;
    int input_cursor_x;
    int input_cursor_y;
    bool show_cursor;
} NmsArgs;
```
* `char *src` should contain a pointer to the string of character you wish to perform the effect on. This is the only member that you need to set prior to calling `nms_exec()`.
* `char *return_opts` shuold contain a string of characters, of which one is required by the user to press before nms_exec will return execution back to the calling function. Only use this if the user must choose an option from a menu that is presented after the text "decrypts". See the included program 'sneakers' as an example.
* `int input_cursor_x` and `int input_cursor_y` - These should contain the x/y coordinates for the cursor if you wish it to be placed at a specific location on screen after the text "decrypts".
* `bool show_cursor` - Set to `true` if you want the cursor to be visible during the text decryption effect.

License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.  See [COPYING](COPYING) for more details.
