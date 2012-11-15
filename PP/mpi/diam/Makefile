all: diam-seq diam-par

diam-seq: diam-seq.c
	gcc -Wall -O3 -o diam-seq diam-seq.c ../table.c

diam-par: diam-par.c
	mpicc -Wall -O3 -o diam-par diam-par.c ../table.c

clean:
	rm -f *.o *core *~ diam-seq diam-par
