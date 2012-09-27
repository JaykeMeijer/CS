CC = gcc
CFLAGS = -O2 -std=c99 -D _GNU_SOURCE
all: serv1 client serv2 serv3 talk

serv1: serv1.c
	$(CC) serv1.c -o serv1 $(CFLAGS)

serv2: serv2.c
	$(CC) serv2.c -o serv2 $(CFLAGS)

serv3: serv3.c
	$(CC) serv3.c -o serv3 $(CFLAGS)

client: client.c
	$(CC) client.c -o client $(CFLAGS)

talk: talk.c talk.h
	$(CC) talk.c talk.h -o talk $(CFLAGS) -pthread

clean:
	rm -f *.o
	rm -f serv1
	rm -f serv2
	rm -f serv3
	rm -f client
	rm -f talk
	rm -f *~*
