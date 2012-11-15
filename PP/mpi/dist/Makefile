all: dist-seq dist-par

dist-seq: dist-seq.c
	gcc -Wall -O3 -o dist-seq dist-seq.c ../table.c

dist-par: dist-par.c
	mpicc -Wall -O3 -o dist-par dist-par.c ../table.c

clean:
	rm -f *.o *core *~ dist-seq dist-par
