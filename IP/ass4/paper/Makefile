CC = gcc
CFLAGS = -O2
RPC = rpcgen
RPCFLAGS = -C

all: rpc papers paperview paperload

rpc: paperserver.x
	$(RPC) $(RPCFLAGS) paperserver.x

papers: papers.c ../setup.h
	$(CC) -c papers.c ../setup.h html.c $(CFLAGS)
	$(CC) -c paperserver_clnt.c $(CFLAGS)
	$(CC) -c paperserver_xdr.c $(CFLAGS)
	$(CC) -o papers.cgi papers.o html.o paperserver_clnt.o paperserver_xdr.o \
		-lnsl $(CFLAGS)

paperview: paperview.c ../setup.h
	$(CC) -c paperview.c ../setup.h html.c $(CFLAGS)
	$(CC) -c paperserver_clnt.c $(CFLAGS)
	$(CC) -c paperserver_xdr.c $(CFLAGS)
	$(CC) -o paperview.cgi paperview.o html.o paperserver_clnt.o \
		paperserver_xdr.o -lnsl $(CFLAGS)

paperload: paperload.c ../setup.h
	$(CC) -c paperload.c ../setup.h html.c $(CFLAGS)
	$(CC) -c paperserver_clnt.c $(CFLAGS)
	$(CC) -c paperserver_xdr.c $(CFLAGS)
	$(CC) -o paperload.cgi paperload.o html.o paperserver_clnt.o \
		paperserver_xdr.o -lnsl $(CFLAGS)

clean:
	rm -f *.o
	rm -f *.cgi
	rm -f paperserver_*
	rm -f paperserver.h
	rm -f *.gch
	rm -f *~
