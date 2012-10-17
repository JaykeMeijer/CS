CC = gcc
JC = javac
RPC = rpcgen
CFLAGS = -O2
RPCFLAGS = -C

all: paperserver paperclient hotelgwclient

rpc: paper/paperserver.x
	$(RPC) $(RPCFLAGS) paper/paperserver.x

paperserver: paper/paperserver.c paper/paperserver.h
	$(CC) -c paper/paperserver.c $(CFLAGS)
	$(CC) -c paper/paperserver_svc.c $(CFLAGS)
	$(CC) -c paper/paperserver_xdr.c $(CFLAGS)
	$(CC) -o paperserver paperserver.o paperserver_svc.o paperserver_xdr.o \
		-lnsl $(CFLAGS)

paperclient: paper/paperclient.c paper/paperserver.h
	$(CC) -c paper/paperclient.c $(CFLAGS)
	$(CC) -c paper/paperserver_clnt.c $(CFLAGS)
	$(CC) -c paper/paperserver_xdr.c $(CFLAGS)
	$(CC) -o paperclient paperclient.o paperserver_clnt.o paperserver_xdr.o \
		-lnsl $(CFLAGS)

hotel:
	hotel/make

hotelgw: hotel

hotelgwclient: hotelgwclient/hotelgwclient.c
	$(CC) -o exe hotelgwclient/hotelgwclient.c $(CFLAGS)

restart_portmap:
	sudo service portmap stop
	sudo rpcbind -wi

clean:
	rm -f *.o
	rm -f paperserver
	rm -f paperclient
	rm -f hotelgwclient/hotelgwclient
