CC=gcc
INCDIR=-I.
LIBDIR=-L.
CFLAGS=-Wall -g
LDFLAGS=-ldyad -lpthread

all: libdyad.a server client

libdyad.a: dyad.o
	ar rcs libdyad.a dyad.o

dyad.o: dyad.c dyad.h
	$(CC) -c dyad.c -o dyad.o

server: server.c
	$(CC) server.c -o server $(INCDIR) $(LIBDIR) $(LDFLAGS) $(CFLAGS)

client: client.c
	$(CC) client.c -o client $(INCDIR) $(LIBDIR) $(LDFLAGS) $(CFLAGS)

.PHONY: clean

clean:
	rm client server dyad.o libdyad.a
