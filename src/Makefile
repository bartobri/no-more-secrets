CC = gcc
ODIR = obj
DEPS = nms.h

_OBJ = nms.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $<

nms: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -f $(ODIR)/*.o nms 
