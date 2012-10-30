JC = javac

all: hotel

hotel: Hotel.java HotelImpl.java hotelserver.java
	$(JC) Hotel.java
	$(JC) HotelImpl.java
	$(JC) hotelserver.java

clean:
	rm -f *.class
	rm -f *~
