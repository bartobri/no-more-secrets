No More Secrets
===============

What is "No More Secrets"?
--------------------------

This project seeks to recreate the infamous "decrypting text effect" seen on screen in the 1992 hacker
movie "Sneakers". If You are unfamiliar with the scene, the specific effect in question appears at
0:35 in [this youtube video](https://www.youtube.com/watch?v=F5bAa6gFvLs).

This project provides 2 tools for you to recreate this effect:

1. A standalone executable file named `nms`.
... This file accepts data via a shell pipe and displays it in a maner that nearly identically mimicks the effect we see in the movie.

2. A module for incorporating this effect in to other programs.

License
-------

This program is free software; you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.  See [COPYING](COPYING) for more details.

Installation
------------
```
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
```

This will create the executable file bin/nms. You may copy it in to your preferred bin directory.

Usage
-----

The `nms` binary accepts data via shell pipe. Simply pipe some data to it.
```
ls -l | nms
```
Once the "encrypted" data is displayed, the program will pause until you press a key. Then the
decryption effect will start. After the descrytion effect is completed, it will again pause until
you press a key, at which point the program will terminate.
