all: asp-seq asp-par

asp-seq: asp-seq.c
	gcc -Wall -O3 -o asp-seq asp-seq.c table.c

asp-par: asp-par.c
	mpicc -Wall -O3 -o asp-par asp-par.c table.c

clean:
	rm -f *.o *core *~ asp-seq asp-par
