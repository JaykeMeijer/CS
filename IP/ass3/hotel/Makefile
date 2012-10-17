JC = javac

all: hotel

hotel: Hotel.java HotelImpl.java hotelserver.java
	$(JC) Hotel.java
	$(JC) HotelImpl.java
	$(JC) hotelserver.java
	$(JC) hotelclient.java

clean:
	rm -f *.class
