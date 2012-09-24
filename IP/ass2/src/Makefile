CC = gcc
CFLAGS = -O2 -std=c99 -D _GNU_SOURCE
all: serv1 client

serv1: serv1.c
	$(CC) serv1.c -o serv1 $(CFLAGS)

client: client.c
	$(CC) client.c -o client $(CFLAGS)

clean:
	rm -f *.o
	rm -f serv1
	rm -f client
	rm -f *~*
