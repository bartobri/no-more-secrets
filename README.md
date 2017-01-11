No More Secrets
===============

"No More Secrets" is the name I've lovingly given to the infamous "decrypting text" effect seen on
screen in the 1992 hacker movie Sneakers. If you are unfamiliar with the effect, it can be seen
at 0:35 in [this youtube video](https://www.youtube.com/watch?v=F5bAa6gFvLs&t=35).

This project provides tools to recreate this effect in your projects.

Here is a standalone executable called `nms` (provided in this project) that uses these tools to
perform this effect on piped data:

![Screenshot](http://www.brianbarto.info/extern/images/nms/nms.gif)

Also included in this project is a program called `sneakers` that utilizes these tools to recreate,
almost identically, what we see in the above movie clip. 

![Screenshot](http://www.brianbarto.info/extern/images/nms/sneakers.gif)

What's Provided
---------------

Two tools are provided for you to recreate this effect:

1. A standalone executable file named `nms` (shown above). This program performs the "decrypting text" effect on data it receives via shell pipe.

2. A module and header file, written in C, that can be used to recreate this effect in other projects. In fact, the standalone executable `nms` is really just an example that shows how to use this module with piped data.

Installing nms (The Standalone Executable)
------------------------------------------

See the [INSTALL](INSTALL.md) file for a list of instructions for various operating systems.

Using nms
---------

`nms` accepts data from a shell pipe. Simply pipe some data to it and enjoy the magic!

```
ls -l / | nms
```

Once the "encrypted" data is displayed, the program will pause until you press a key. Then the
decryption effect will start. After that is completed, it will again pause until
you press a key, at which point the program will terminate.

#### Command Line Options

Use the `-a` option to set the auto_decrypt flag. This will automatically start the decryption effect,
eliminating the need for the user to press a key to start it.
```
ls -l / | nms -a
```

Use the `-f` option to set foreground color to either white, yellow, black, magenta, blue, green, or
red - this is blue by default.
```
ls -l / | nms -f green
```

Using the nms Module in Your Project
------------------------------------

NOTE: Be sure that you have the ncurses library installed on your system. See "Installing nms" above for more info.

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

#### How To Use the nms Module

Copy these two files to your project:

```
nms.h
nms.c
```

Include `nms.h` in your program file:

```
#include "nms.h"
```

Next, declare and initialize the nms structure:

```
NmsArgs args = INIT_NMSARGS;
```

INIT_NMSARGS is a symbolic constant that should be used to assign a default
set of values to all of the structure members. If you don't use it,  you
will have to manually assign a value to each
structure member.

Here is how the structure is defined:
```
typedef struct {
    char *src;
    char *foreground_color;
    char *return_opts;
    int input_cursor_x;
    int input_cursor_y;
    bool show_cursor;
    bool auto_decrypt;
} NmsArgs;
```

Assign values to the structure members as needed.

**char *src** - Pointer to the string of characters on which to perform the effect.

**char *foreground_color** - Pointer to a string containing the desired foreground color: white, yellow, black, magenta, blue, green, red.

**char *return_opts** - String pointer containing only the character options that the user must choose from once the `src` characters are revealed. For example, if you are showing a menu with six options, this string might be "123456". The user will have to choose one of these characters before execution is handed back to the calling function. Note that the character selected is returned by `nms_exec()`.

**int input_cursor_x, input_cursor_y** - If your menu has a specific location that you'd like to place the cursor for user input, use these to set the x and y screen coordinates for the position.

**bool show_cursor** - Set to `true` if you want the cursor to be visible during the text decryption effect. It is set to `false` by default.

**bool auto_decrypt** - Set to `true` to automatically start the decryption effect, eliminating the need for the user to press a key to start it.

Once you set the structure members as desired, simply pass a pointer to the structure to the nms_exec() function:

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
