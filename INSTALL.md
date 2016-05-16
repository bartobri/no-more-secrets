Installation
------------

Below are installation instructions for various operating systems. Use the set of instructions that
best applies to you. If your operating system is missing from the list, see 'Generic Instructions'
at the bottom for help.

If you can supply instructions for an operating system that is not listed here, please email me or
submit a pull request updating this document.

#### Ubuntu/Debian/Mint Linux

```
sudo apt-get update
sudo apt-get install lib32ncurses5-dev lib32ncursesw5-dev
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
sudo make install
```

#### Fedora Linux

```
sudo dnf update --refresh
sudo dnf install ncurses-devel
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
sudo make install
```

#### Redhat/Cent OS Linux

```
sudo yum update
sudo yum install ncurses-devel
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
sudo make install
```

#### Arch Linux

```
sudo pacman -Sy
sudo pacman -S ncurses
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
sudo make install
```

#### Mac OSX

Install the Homebrew package manager by following the instructions on the website: http://brew.sh/

Once Homebrew is installed, you can install no-more-secrets:

```
brew install no-more-secrets
```

#### Generic Instructions

First, make sure you have the ncurses library installed:
```
ls /usr/include | grep ncurses.h
```
If the ncurses.h header file is not present, you may need to install the library. For example, on
Ubuntu you would type:
```
sudo apt-get update
sudo apt-get install lib32ncurses5-dev lib32ncursesw5-dev
```
For other flavors of Linux/Unix, use your package manager to search for the "ncurses" package. Install
it. If you see two different packages for "ncurses" and "ncursesw", install them both (as shown above
in the Ubuntu example).

Next, download and build the standalone executable:
```
git clone https://github.com/bartobri/no-more-secrets.git
cd ./no-more-secrets
make
sudo make install
```

[Back to README](README.md)
