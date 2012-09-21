CC = gcc
CFLAGS = -O2 -std=c99
FLAGS_SYN = -pthread -D _BSD_SOURCE
JC = javac

all: mysh1 mysh2 mysh3 synthread1 synthread2 syn1 syn2

mysh1: mysh1.c mysh1.h
	$(CC) mysh1.c -o mysh1 $(CFLAGS)

mysh2: mysh2.c mysh2.h
	$(CC) mysh2.c -o mysh2 $(CFLAGS)

mysh3: mysh3.c mysh3.h
	$(CC) mysh3.c -o mysh3 $(CFLAGS)

syn1: syn1.c syn1.h
	$(CC) syn1.c -o syn1 $(CFLAGS) -D _BSD_SOURCE -D _SVID_SOURCE

syn2: syn2.c syn2.h
	$(CC) syn2.c -o syn2 $(CFLAGS) -D _BSD_SOURCE -D _SVID_SOURCE

synthread1: synthread1.c synthread1.h
	$(CC) synthread1.c -o synthread1 $(CFLAGS) $(FLAGS_SYN)

synthread2: synthread2.c synthread2.h
	$(CC) synthread2.c -o synthread2 $(CFLAGS) $(FLAGS_SYN)

syn1j: syn1.java
	$(JC) syn1.java

syn2j: syn2.java
	$(JC) syn2.java

clean:
	rm -f *.o
	rm -f mysh1
	rm -f mysh2
	rm -f mysh3
	rm -f syn1
	rm -f syn2
	rm -f synthread1
	rm -f synthread2
	rm -f *.class
	rm -f *~*
