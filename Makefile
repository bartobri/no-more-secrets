# Installation directories following GNU conventions
prefix = /usr/local
exec_prefix = $(DESTDIR)$(prefix)
bindir = $(exec_prefix)/bin
sbindir = $(exec_prefix)/sbin
datarootdir = $(DESTDIR)$(prefix)/share
datadir = $(datarootdir)
includedir = $(DESTDIR)$(prefix)/include
mandir = $(datarootdir)/man

BIN=bin
OBJ=obj
SRC=src

CC = gcc
CFLAGS = -Wextra -Wall
LDLIBS = -lncursesw
NCURSES_H = /usr/include/ncurses.h

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
	else \
		sudo apt-get update ; \
		sudo apt-get install lib32ncurses5-dev lib32ncursesw5-dev ; \
	fi

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)

install:
	install -d $(bindir)
	cd $(BIN) && install $(EXES) $(bindir)

uninstall:
	for exe in $(EXES); do rm $(bindir)/$$exe; done

