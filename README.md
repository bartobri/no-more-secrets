No More Secrets
===============

"No More Secrets" is the name I've lovingly given to the infamous "decrypting text" effect seen on
screen in the 1992 hacker movie Sneakers. If you are unfamiliar with the effect, it can be seen
at 0:35 in [this youtube video](https://www.youtube.com/watch?v=F5bAa6gFvLs).

This project provides tools to recreate this effect in your projects.

Here is a standalone executable called `nms` (provided in this project) that uses these tools to
perform this effect on piped data:

![Screenshot](http://i.imgur.com/ezF3xkN.gif)

Here is another example program called "sneakers" (type `make sneakers` to build) that
utilizes these tools to recreate, almost identically, what we see in the above movie clip.

![Screenshot](http://www.tackboard.world/no_more_secrets.gif)

What's Provided
---------------

Two tools are provided for you to recreate this effect:

1. A standalone executable file named `nms` (shown above). This program performs the "decrypting text" effect on data it receives via shell pipe.

2. A module and header file, written in C, that can be used to recreate this effect in other projects. In fact, the standalone executable `nms` is really just an example that shows how to use this module with piped data.

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

Using the Module in Your Program
---------------------------------

#### Synopsis

```
// myprog.c

#include "nms.h"

int main() {
    NmsArgs args = INIT_NMSARGS;

    args.src = "This is a simple test";

    nms_exec(&args);

    return 0;
}

```
Compile myprog.c (must include `nms.c` and `-lncurses`):
```
gcc nms.c myprog.c -o myprog -lncurses
```

#### How-To

Copy these two files to your project:

```
nms.h
nms.c
```
Include `nms.h` in your program file:

```
#include "nms.h"
```
Next, declare and initialize the structure that needs to be passed to `nms_exec()`:
```
NmsArgs args = INIT_NMSARGS;
```
INIT_NMSARGS is a defined name that assigns a default set of values to all of the structure members. It
is recommended that you use it. If you don't,  you will have to manually assign a value to each
structure member.

Here is how the structre is defined:
```
typedef struct {
    char *src;
    char *return_opts;
    int input_cursor_x;
    int input_cursor_y;
    bool show_cursor;
} NmsArgs;
```
* `char *src`
   Pointer to the string of characters on which to perform the effect.

Useful for displaying menus:

* `char *return_opts`
   String pointer containg only the character options that the user must choose from once the src characters are revealed. For example, if you are showing a menu with six options, this string might be "123456". The user will have to choose one of these characters before execution is handed back to the calling function. Note that the character selected is returned by `nms_exec()`;
* `int input_cursor_x` and `int input_cursor_y`
   If your menu has a specific location that you'd like to place the cursor for user input, use these to set the x and y screen corrdinates for the position.
* `bool show_cursor`
   Set to `true` if you want the cursor to be visible during the text decryption effect. It is set to `false` by default.

Assign values to the structure members as needed. Then simply pass a pointer to the structure to the
nms_exec() function:
```
nms_exec(&args);
```
Note that nms_exec() prompts the user to press a key to start the "decrypting text" effect, and again
once the text has been fully revealed. The key that is pressed at the second prompt is returned to the
calling function so that it can be used as input after displaying a menu. 

#### Compiling

Add `nms.c` to your source file list, and link the ncurses library `-lncurses` when compiling:
```
gcc nms.c myprog.c -o myprog -lncurses
```

License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.  See [COPYING](COPYING) for more details.
