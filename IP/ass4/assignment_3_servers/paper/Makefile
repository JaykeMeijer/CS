CC = gcc
CFLAGS = -O2
RPC = rpcgen
RPCFLAGS = -C

all: rpc paperserver

rpc: paperserver.x
	$(RPC) $(RPCFLAGS) paperserver.x

paperserver: paperserver.c
	$(CC) -c paperserver.c $(CFLAGS)
	$(CC) -c paperserver_svc.c $(CFLAGS)
	$(CC) -c paperserver_xdr.c $(CFLAGS)
	$(CC) -o paperserver paperserver.o paperserver_svc.o paperserver_xdr.o \
		-lnsl $(CFLAGS)

clean:
	rm -f *.o
	rm -f paperserver
	rm -f paperserver_*
	rm -f paperserver.h
	rm -f *~
