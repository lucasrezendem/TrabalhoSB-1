CC = gcc
CFLAGS = -Wall -ansi
DEPS = passagens.h pre_mcr.h
OBJ = passagens.o pre_mcr.o montador.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

montador: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f passagens.o
	rm -f pre_mcr.o
	rm -f montador.o