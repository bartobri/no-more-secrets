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

#### Module Details

Copy these two files to your project:

```
nms.h
nms.c
```
Include `nms.h` in your program file:

```
#include <nms.h>
```
You only need to call one function from inside your program to create this effect. It is defined as
such:
```
char nms_exec(NmsArgs *);
```
This function only takes one argument: a pointer to a structure that contains arguments for
nms_exec(). So next, declare the structure (substitute 'args' with whatever name you desire):
```
NmsArgs args = INIT_NMSARGS;
```
INIT_NMSARGS substitutes a default set of values for all 5 structure members. As you see in the
synopsis above, only the member that contains the text is needed. The others can be left with
their default setting, and are only useful if the text you are "decrypting" reveals a menu from
which the user must choose an option. here is the structure definition:
```
typedef struct {
    char *src;
    char *return_opts;
    int input_cursor_x;
    int input_cursor_y;
    bool show_cursor;
} NmsArgs;
```
* `char *src` - Pointer to the string of character to perform the effect on.

Useful for displaying menus only:

* `char *return_opts` - String pointer containg only the character options that the user must choose from. For example, if you are showing six menu options, this string might be "123456". The user will have to choose one of these characters before execution is handed back to the calling function. Note that the character selected is returned by `nms_exec()`;
* `int input_cursor_x` and `int input_cursor_y` - If your menu has a specific location that you'd like to place the cursor for user input, use these to set the x and y screen corrdinates for the position.
* `bool show_cursor` - Set to `true` if you want the cursor to be visible during the text decryption effect. It is set to `false` by default.

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
