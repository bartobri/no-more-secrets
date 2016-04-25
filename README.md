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



License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.  See [COPYING](COPYING) for more details.
