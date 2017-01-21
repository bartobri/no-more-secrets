![Version](https://img.shields.io/badge/Version-0.3.0-green.svg)

No More Secrets
===============

This project provides a command line tool called `nms` that recreates the
famous data decryption effect seen on screen in the 1992 hacker movie Sneakers.
For reference, you can see this effect at 0:35 in [this movie clip](https://www.youtube.com/watch?v=F5bAa6gFvLs&t=35).

This command works on piped data. Pipe any ASCII or UTF-8 text to `nms`,
and it will apply the hollywood effect, initially showing encrypted data,
then starting a decryption sequence to reveal the original plaintext characters.

![Screenshot](http://www.brianbarto.info/extern/images/nms/nms.gif)

Also included in this project is a program called `sneakers` that recreates
what we see in the above movie clip. Note that this program requires the
user to select one of the menu options before it terminates.

![Screenshot](http://www.brianbarto.info/extern/images/nms/sneakers.gif)

By default, this project has no dependencies, but it does rely on ANSI/VT100
terminal escape sequences to recreate the effect. Most modern terminal
programs support these sequences so this should not be an issue for most
users. If yours does not, this project also provides a ncurses implementation
which supports non-ANSI terminals, but at the expense of losing the inline
functionality (ncurses will always clear the screen prior to displaying output).

Table of Contents
-----------------

1. [Download and Install](#download-and-install)
2. [Usage](#usage)
3. [The NMS Library](#the-nms-library)
4. [License](#license)

Download and Install
--------------------

More and more unix/linux platforms are including this project in their
package manager. You may wish to search your package manager to see if it
is an installation option. If you install form a package manager, please
check that you have the latest version (`nms -v`). If not, I suggest
installing from source by following the instructions below.

To install this project from source, you will need to have the tools `git`,
`gcc`, and `make` to download and build it. Install them from your package
manager if they are not already installed.

Once you have the necessary tools installed, follow these instructions:

#### Install:
```
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make nms
make sneakers             ## Optional
sudo make install
```

#### Uninstall:

```
$ sudo make uninstall
```

#### Install with Ncurses Support

If your terminal does not support ANSI/VT100 escape sequences, the effect
may not render properly. This project provides a ncurses implementation
for such cases. You will need the ncurses library installed. Install this
library from your package manager. Next, follow these instructions:

```
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make nms-ncurses
make sneakers-ncurses     ## Optional
sudo make install
```

Usage
-----

`nms` works on piped data. Pipe any ASCII or UTF-8 characters to it and
enjoy the magic. In the below examples, I use a simple directory listing.

```
ls -l | nms
ls -l | nms -a           // Set auto-decrypt flag
ls -l | nms -f green     // Set foreground color to green
ls -l | nms -c           // Clear screen
nms -v                   // Display version
```

Note that by default, after the initial encrypted characters are displayed,
`nms` will wait for the user to press a key before initiating the decryption
sequence. This is how the it is depicted in the movie.

#### Command Line Options

`-a`

Set the auto-decrypt flag. This will automatically start the
decryption sequence without requiring a key press.

`-f <color>`

Set the foreground color of the decrypted text to the color
specified. Valid options are white, yellow, black, magenta, blue, green,
or red. This is blue by default.

`-c`

Clear the screen prior to printing any output. Specifically,
it saves the state of the terminal (all current output), and restores it
once the effect is comlpeted. Note that when using this option, `nms` requires
the user to press a key before restoring the terminal.

`-v`

Display version info.

The NMS Library
---------------

For those who would like to use this effect in their own projects, I have
created a C library that provides simple interface and can easily be used
for any program that runs from the command line.

See [LibNMS](https://github.com/bartobri/libnms) for more info.

License
-------

This program is free software; you can redistribute it and/or modify it
under the terms of the the MIT License (MIT). See [LICENSE](LICENSE) for
more details.
