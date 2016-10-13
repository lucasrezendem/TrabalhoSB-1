CC = gcc
CFLAGS = -Wall -ansi

all:
	$(CC) $(CFLAGS) -c passagens.c
	$(CC) $(CFLAGS) -o main entryPoint.c passagens.o

.PHONY: clean

clean:
	rm -f passagens.o
	rm -f main
