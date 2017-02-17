# Installation directories following GNU conventions
prefix ?= /usr/local
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

CC ?= gcc
CFLAGS ?= -Wextra -Wall

.PHONY: all install uninstall clean

nms: $(OBJ)/nmscharset.o $(OBJ)/nmstermio.o $(OBJ)/nmseffect.o $(OBJ)/nms.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^

sneakers: $(OBJ)/nmscharset.o $(OBJ)/nmstermio.o $(OBJ)/nmseffect.o $(OBJ)/sneakers.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/$@ $^

all: nms sneakers

all-ncurses: nms-ncurses sneakers-ncurses

nms-ncurses: $(OBJ)/nmscharset.o $(OBJ)/nmstermio_ncurses.o $(OBJ)/nmseffect.o $(OBJ)/nms.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/nms $^ -lncurses

sneakers-ncurses: $(OBJ)/nmscharset.o $(OBJ)/nmstermio_ncurses.o $(OBJ)/nmseffect.o $(OBJ)/sneakers.o | $(BIN)
	$(CC) $(CFLAGS) -o $(BIN)/sneakers $^ -lncurses

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -o $@ -c $<

$(BIN):
	mkdir $(BIN)

$(OBJ):
	mkdir $(OBJ)

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)

install:
	install -d $(DESTDIR)$(mandir)/man6
	install -m644 -t $(DESTDIR)$(mandir)/man6 nms.6 sneakers.6
	install -d $(DESTDIR)$(bindir)
	cd $(BIN) && install * $(DESTDIR)$(bindir)

uninstall:
	rm -f $(DESTDIR)$(bindir)/nms;
	rm -f $(DESTDIR)$(bindir)/sneakers;
	rm -f $(DESTDIR)$(mandir)/man6/nms.6
	rm -f $(DESTDIR)$(mandir)/man6/sneakers.6
