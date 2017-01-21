NCURSES Install
===============

Below are instructions for installing ncurses on various Linux and Unix
operating systems.

#### Ubuntu/Debian/Mint Linux

```
sudo apt-get update
sudo apt-get install lib32ncurses5-dev
```

#### Fedora Linux

```
sudo dnf update --refresh
sudo dnf install ncurses-devel
```

#### Redhat/Cent OS Linux

```
sudo yum update
sudo yum install ncurses-devel
```

#### Arch Linux

```
sudo pacman -Sy
sudo pacman -S ncurses
```

#### openSUSE

```
sudo zypper ref
sudo zypper in ncurses-devel
```

#### Mac OSX

Install the Homebrew package manager by following the instructions on the
website: http://brew.sh/. Once Homebrew is installed, you can install ncurses.

```
brew install ncurses
```


