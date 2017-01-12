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

Table of Contents
-----------------

1. [Download and Install](#download-and-install)
2. [Usage](#usage)
3. [License](#license)

Download and Install
--------------------

In order to download and build this project, you will need to have `git`,
`gcc`, and `make` installed. Install them from your package manager if not
already installed.

```
$ which make
/usr/bin/make

$ which gcc
/usr/bin/gcc

$ which git
/usr/bin/git
```

See the [INSTALL](INSTALL.md) file for a list of instructions for various
operating systems.

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
```

#### Command Line Options

`-a`

This option sets the auto-decrypt flag. This will automatically start the
decryption sequence without a key press.

`-f color`

This option sets the foreground color of the decrypted text to the color
specified. Valid options are white, yellow, black, magenta, blue, green,
or red. This is blue by default.

License
-------

This program is free software; you can redistribute it and/or modify it
under the terms of the the MIT License (MIT). See [LICENSE](LICENSE) for
more details.
