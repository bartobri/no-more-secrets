BIN=bin
OBJ=obj
SRC=src

CC = gcc
CFLAGS = -Wextra -Wall
LDLIBS = -lncurses
NCURSES_H = /usr/include/ncurses.h
UNAME = $(shell uname)

all: nms sneakers

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
ifeq ($(UNAME), Linux)
	sudo apt-get update
	sudo apt-get install ncurses-dev
endif
ifeq ($(UNAME), CentOS)
	sudo yum update
	sudo yum install ncurses-dev
endif
ifeq ($(UNAME), Fedora)
	sudo dnf update --refresh
	sudo dnf install ncurses-dev
endif

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
