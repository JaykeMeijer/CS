CC = gcc
CFLAGS = -O2 -std=c99

all: mysh1 mysh2 mysh3

mysh1: mysh1.o
	$(CC) mysh1.c -o mysh1 $(CFLAGS)

mysh2: mysh2.o
	$(CC) mysh2.c -o mysh2 $(CFLAGS)
	
mysh3:
	$(CC) mysh3.c -o mysh3 $(CFLAGS)

clean: 	
	rm -f *.o
	rm -f mysh1
	rm -f mysh2
	rm -f mysh3

mysh1.o: mysh1.c mysh1.h
mysh2.o: mysh2.c mysh2.h
mysh3.o: mysh3.c mysh3.h
