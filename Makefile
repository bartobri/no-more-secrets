# Installation directories following GNU conventions
prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
sbindir = $(exec_prefix)/sbin
datarootdir = $(prefix)/share
datadir = $(datarootdir)
includedir = $(prefix)/include
mandir = $(datarootdir)/man

BIN=bin
OBJ=obj
SRC=src

CC = gcc
CFLAGS = -Wextra -Wall
LDLIBS = -lncurses

UNAME := $(shell uname)

NCURSES_H = /usr/include/ncurses.h

ifeq ($(UNAME), Darwin)
	NCURSES_H := /usr/local/Cellar/ncurses/6.0_1/include/ncurses.h
endif

.PHONY: all install uninstall clean

EXES = nms sneakers
all: $(EXES)

nms: $(OBJ)/nms.o $(OBJ)/main.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^ $(LDLIBS)

sneakers: $(OBJ)/nms.o $(OBJ)/sneakers.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^ $(LDLIBS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ) $(NCURSES_H)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BIN):
	mkdir $(BIN)

$(OBJ):
	mkdir $(OBJ)

$(NCURSES_H):
	if [ -a /etc/fedora-release ] ; \
	then \
		sudo dnf update --refresh ; \
		sudo dnf install ncurses-devel ; \
	elif [ -a /etc/redhat-release ] ; \
	then \
		sudo yum update ; \
		sudo yum install ncurses-devel ; \
	elif [ -a /etc/arch-release ] ; \
	then \
		sudo pacman -Sy ; \
		sudo pacman -S ncurses ; \
	elif [ -a /usr/local/bin/brew ] ; \
	then \
		brew install homebrew/dupes/ncurses ; \
	else \
		sudo apt-get update ; \
		sudo apt-get install ncurses-dev ; \
	fi

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)

install:
	install -d $(bindir)
	cd $(BIN) && install $(EXES) $(bindir)

uninstall:
	for exe in $(EXES); do rm $(bindir)/$$exe; done

