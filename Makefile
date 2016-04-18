BIN=bin
OBJ=obj
SRC=src

CC = gcc
LDLIBS = -lncurses

${BIN}/nms: ${OBJ}/nms.o
	$(CC) $(LDLIBS) -o $@ $^

${OBJ}/nms.o: ${SRC}/nms.c ${SRC}/nms.h
	$(CC) -o $@ -c ${SRC}/nms.c

clean:
	rm -f ${BIN}/*
	rm -f $(OBJ)/*
