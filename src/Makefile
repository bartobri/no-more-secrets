CC=gcc
ODIR=obj

_OBJ = nms.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $<

nms: $(OBJ)
	$(CC) -o $@ $^

clean:
	rm -f $(ODIR)/*.o nms 
