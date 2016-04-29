BIN=bin
OBJ=obj
SRC=src

CC = gcc
LDLIBS = -lncurses

MKDIR     ?= $(INSTALL) -d
BINDIR    ?= $(PREFIX)/bin
DESTDIR   ?=

${BIN}/nms: ${OBJ}/nms.o ${OBJ}/main.o
	$(CC) -o $@ $^ $(LDLIBS)

sneakers: ${OBJ}/nms.o ${OBJ}/sneakers.o
	$(CC) -o ${BIN}/$@ $^ $(LDLIBS)

${OBJ}/main.o: ${SRC}/main.c ${SRC}/nms.h
	$(CC) -o $@ -c ${SRC}/main.c

${OBJ}/sneakers.o: ${SRC}/sneakers.c ${SRC}/nms.h
	$(CC) -o $@ -c ${SRC}/sneakers.c

${OBJ}/nms.o: ${SRC}/nms.c ${SRC}/nms.h
	$(CC) -o $@ -c ${SRC}/nms.c

.PHONY: clean

clean:
	rm -f ${BIN}/*
	rm -f $(OBJ)/*

install:
	$(MKDIR) $(DESTDIR)$(BINDIR)
	install ${BIN}/nms$(EXE) $(DESTDIR)$(BINDIR)/
