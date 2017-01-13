No More Secrets
===============

This project provides a command line tool called `nms` that recreates the
famous data decryption effect seen on screen in the 1992 hacker movie Sneakers.
For reference, you can see this effect at 0:35 in [this youtube video](https://www.youtube.com/watch?v=F5bAa6gFvLs&t=35).

This command works on piped data. Pipe any ASCII or UTF-8 text to `nms`,
and it will apply the effect to it, initially showing "encrypted" data,
then starting a decryption sequence to reveal the original plaintext data.

![Screenshot](http://www.brianbarto.info/extern/images/nms/nms.gif)

Also included in this project is a program called `sneakers` that recreates
what we see in the above movie clip. Note that this program requires the
user to select one of the menu options before it terminates.

![Screenshot](http://www.brianbarto.info/extern/images/nms/sneakers.gif)

This project has no dependencies, but it does rely on ANSI/VT100 escape
sequences to recreate the effect. Most modern terminal programs support
these sequences so this should not be an issue for most users. If yours
does not, you may need to use an alternate terminal program like xTerm.

Table of Contents
-----------------

1. [Download and Install](#download-and-install)
2. [Usage](#usage)
3. [License](#license)

Download and Install
--------------------

I am flattered that few *NIX platforms have added this project to their
package manager collection. If you're using one of the following systems,
you can install it from the package manager.

* openSUSE
* Mac OS X (Via homebrew package manager: http://brew.sh/)
* FreeBSD

If you install form a package manager, please check that you have the
latest version (`nms -v`). If not, I suggest installing from source (see
below).

To install this project from source, you will need to have the tools `git`,
`gcc`, and `make` to download and build it. Install them from your package
manager if they are not available.

Once you have the necessary tools installed, follow these instructions:

#### Install:
```
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
sudo make install
```

#### Uninstall:

```
$ sudo make uninstall
```

Usage
-----

`nms` works on piped data. All ASCII and UTF-8 character sets should be
supported.

After the initial "encrypted" characters are displayed, `nms` will wait
for the user to press a key before it starts the decryption sequence.

```
ls -l / | nms
ls -l / | nms -a           // Set auto-decrypt flag
ls -l / | nms -f green     // Set foreground color to green
ls -l / | nms -c           // Clear screen
nms -v                     // Display version
```

#### Command Line Options

`-a`

This option sets the auto-decrypt flag. This will automatically start the
decryption sequence without a key press.

`-f color`

This option sets the foreground color of the decrypted text to the color
specified. Valid options are white, yellow, black, magenta, blue, green,
or red. This is blue by default.

`-c`

This option clears the screen prior to printing any output. Specifically,
it saves the state of the terminal (all current output), and restores it
once the effect is comlpeted. Note that when using this option, `nms` requires
the user to press a key before restoring the terminal.

`-v`

Display version info.

License
-------

This program is free software; you can redistribute it and/or modify it
under the terms of the the MIT License (MIT). See [LICENSE](LICENSE) for
more details.
