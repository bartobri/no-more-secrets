BIN=bin
OBJ=obj
SRC=src

CC = gcc
CFLAGS = -lncurses

${BIN}/nms: ${OBJ}/nms.o
	$(CC) $(CFLAGS) -o $@ $^

${OBJ}/nms.o: ${SRC}/nms.c ${SRC}/nms.h
	$(CC) $(CFLAGS) -o $@ -c ${SRC}/nms.c

clean:
	rm -f ${BIN}/*
	rm -f $(OBJ)/*
