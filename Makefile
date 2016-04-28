BIN=bin
OBJ=obj
SRC=src

CC = gcc
CFLAGS = -Wextra -Wall
LDLIBS = -lncurses
NCURSES_H = /usr/include/ncurses.h

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
	sudo apt-get update
	sudo apt-get install ncurses-dev

clean:
	rm -rf $(BIN)
	rm -rf $(OBJ)
