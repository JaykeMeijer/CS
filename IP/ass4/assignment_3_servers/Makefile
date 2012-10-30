CC = gcc
JC = javac
CFLAGS = -O2

all: paper hotelgw hotel

paper: paper/paperserver.c
	$(MAKE) -C paper

hotel: hotel/hotelserver.java hotel/Hotel.java
	$(MAKE) -C hotel

hotelgw: hotelgw/Hotel.java hotelgw/hotelgw.java
	$(MAKE) -C hotelgw

restart_portmap:
	sudo service portmap stop
	sudo rpcbind -wi

clean:
	$(MAKE) clean -C paper
	$(MAKE) clean -C hotel
	$(MAKE) clean -C hotelgw
	rm -f *~

.PHONY: hotel paper hotelgw
