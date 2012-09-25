CC = gcc
CFLAGS = -O2 -std=c99 -D _GNU_SOURCE
all: serv1 client

serv1: serv1.c
	$(CC) serv1.c -o serv1 $(CFLAGS)

serv2: serv2.c
	$(CC) serv2.c -o serv2 $(CFLAGS)

serv3: serv3.c
	$(CC) serv3.c -o serv3 $(CFLAGS)

client: client.c
	$(CC) client.c -o client $(CFLAGS)

talk: talk.c
	$(CC) talk.c -o talk $(CFLAGS) -pthread

clean:
	rm -f *.o
	rm -f serv1
	rm -f serv2
	rm -f client
	rm -f *~*
