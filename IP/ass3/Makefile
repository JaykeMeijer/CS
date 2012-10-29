CC = gcc
JC = javac
CFLAGS = -O2

all: paper hotelgwclient hotelgw hotel

paper: paper/paperserver.c
	$(MAKE) -C paper

hotel: hotel/hotelclient.java hotel/hotelserver.java hotel/Hotel.java \
		hotel/HotelImpl.java
	$(MAKE) -C hotel

hotelgwclient: hotelgwclient/hotelgwclient.c
	$(MAKE) -C hotelgwclient

hotelgw: hotelgw/Hotel.java hotelgw/hotelgw.java
	$(MAKE) -C hotelgw

restart_portmap:
	sudo service portmap stop
	sudo rpcbind -wi

clean:
	$(MAKE) clean -C paper
	$(MAKE) clean -C hotel
	$(MAKE) clean -C hotelgwclient
	$(MAKE) clean -C hotelgw
	rm -f *~

.PHONY: hotel hotelgwclient paper hotelgw
