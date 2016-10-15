CC = gcc
CFLAGS = -Wall -ansi -I.
DEPS = passagens.h
OBJ = passagens.o entryPoint.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f passagens.o
	rm -f entryPoint.o