No More Secrets
===============

What is "No More Secrets"?
--------------------------

This project seeks to recreate the infamous "decrypting text effect" seen on screen in the 1992 hacker
movie "Sneakers". If You are unfamiliar with the scene, the specific effect in question appears at
0:35 in [this youtube video](https://www.youtube.com/watch?v=F5bAa6gFvLs).

What's Provided
---------------

This project provides 2 tools for you to recreate this effect:

1. A standalone executable file named `nms`. This file accepts data from a shell pipe and displays it in a manner that is nearly identical to the effect we see in the movie.

2. A module, written in C, that can be used for incorporating this effect in to other programs.

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

License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.  See [COPYING](COPYING) for more details.
