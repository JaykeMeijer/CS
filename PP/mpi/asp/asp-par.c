/*
 * asp-par.c:
 *     All-pairs shortest path implementation based on Floyd's
 *     algorithms. Also diameter and distance calculation.
 *
 *      Parallel version.
 *
 * Modified by Jayke Meijer, 15-nov-2012
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mpi.h"
#include "table.h"

/******************** Distance calculation *************************/
double do_dist(int **tab, int n, int myid, int no_proc, int *start, int *lines)
{
    int i, j;
    double total = 0;
    double received_total = 0;

    // Calculate total for my part of the table
    for(i = start[myid]; i < start[myid] + lines[myid]; i++)
        for(j = 0; j < n; j++)
            if(tab[i][j] != MAX_DISTANCE)
                total += tab[i][j];

    // Determine total of entire table
    MPI_Reduce(&total, &received_total, no_proc, MPI_DOUBLE, MPI_SUM, 0,
        MPI_COMM_WORLD);

    return received_total;
}

/******************** ASP calculation *************************/
void do_asp(int **tab, int n, int myid, int no_proc, int *start, int *lines)
{
    int i, j, k, tmp;

    // Run the ASP algorithm. Update every pass.
    for (k = 0; k < n; k++) {
        // Perform a calculation pass over my part of the table
        for (i = start[myid]; i < start[myid] + lines[myid]; i++) {
            //printf("Proces %d calc line %d\n", myid, i);
            if (i != k) {
                for (j = 0; j < n; j++) {
                    tmp = tab[i][k] + tab[k][j];
                    if (tmp < tab[i][j])
                        tab[i][j] = tmp;
                }
            }
        }

        // Send the just calculated data to the other processes
        for(i = 0; i < no_proc; i++)
            for(j = start[i]; j < start[i] + lines[i]; j++)
                MPI_Bcast(tab[j], n, MPI_INT, i, MPI_COMM_WORLD);
    }
}

/******************** Diam calculation *************************/
int do_diam(int **tab, int n, int myid, int no_proc, int *start, int *lines)
{
    int i, j, longest = 0;
    int received_longest;

    // Find the longest path in my part of the table
    for(i = start[myid]; i < start[myid] + lines[myid]; i++)
        for(j = 0; j < n; j++)
            if(tab[i][j] != 0 && tab[i][j] > longest)
                longest = tab[i][j];

    // Determine longest of the entire table
    MPI_Reduce(&longest, &received_longest, no_proc, MPI_INT, MPI_MAX, 0,
        MPI_COMM_WORLD);

    return received_longest;
}

void usage() {
        printf ("Run the asp program with the following parameters. \n");
        printf (" -read filename :: reads the graph from a file.\n");
        printf (" -random N 0/1 :: generates a NxN graph, randomly. \n");
        printf ("               :: if 1, the graph is oriented, otherwise it is not oriented\n");
        return ;
}


int main ( int argc, char *argv[] ) {
    int id;
    int ierr;
    int p=3;
    double wtime = 0, total;


    int n,m, bad_edges=0, oriented=0, i, diameter, leftover, lines;
    int **tab;
    int print = 0;
    char FILENAME[100];

    int *process_lines, *process_start;

    /******************** MPI Initialisation *************************/
    ierr = MPI_Init ( &argc, &argv );
    if(ierr != MPI_SUCCESS) {
            perror("Error with initializing MPI");
            exit(1);
    }

    // Get the number of processes.
    ierr = MPI_Comm_size ( MPI_COMM_WORLD, &p );
    // Get the individual process ID.
    ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &id );


    /******************** Problem Initialisation *************************/
    // Process 0 reads data + prints welcome
    n = 0;
    if (id==0) {
        for(i=1; i<argc; i++) {
            if(!strcmp(argv[i], "-print")) {
                        print = 1;
            } else if (!strcmp(argv[i], "-read")) {
                strcpy(FILENAME, argv[i+1]); i++;
            } else if (!strcmp(argv[i], "-random")) {
                n = atoi(argv[i+1]);
                oriented = atoi(argv[i+2]); i+=2;
            } else {
                n = 4000;
                oriented = 1;
            }
        }

        if (n>0)
            init_tab(n,&m,&tab,oriented); // last one = oriented or not ... 
        else
            bad_edges = read_tab(FILENAME, &n, &m, &tab, &oriented);

        fprintf(stderr, "Running program with %d rows and %d edges (%d are bad)\n",
        n, m, bad_edges);

        fprintf(stderr, "The number of processes is %d.\n", p);
    }

    // Distribute the problem size to all machines
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Malloc the table, which was not yet done for the other processes
    if(id != 0)
        malloc_tab(n, &tab);

    // Distribute the table to all machines.
    for(i = 0; i < n; i++)
        MPI_Bcast(tab[i], n, MPI_INT, 0, MPI_COMM_WORLD);

    // Determine the lines and start per process
    process_lines = malloc(p * sizeof(int));
    process_start = malloc(p * sizeof(int));
    leftover = n % p;
    lines = n / p;

    // Determine the number of lines and the startposition for each process
    for(i = 0; i < p; i++) {
        process_lines[i] = i < leftover ? lines + 1 : lines;
        process_start[i] = i * lines + (i < leftover ? i : leftover);
    }

    /******************** Distance calculation *************************/
    if(id == 0)
        wtime = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    total = do_dist(tab, n, id, p, process_start, process_lines);

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        fprintf (stderr, "Distance took %10.3f seconds.\n", wtime);
    }

    /******************** ASP *************************/
    if(id == 0)
        wtime = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    do_asp(tab, n, id, p, process_start, process_lines);

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        fprintf (stderr, "ASP took %10.3f seconds.\n", wtime);
    }

    /******************** Diameter *************************/
    if(id == 0)
        wtime = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    diameter = do_diam(tab, n, id, p, process_start, process_lines);

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        fprintf (stderr, "Diameter took %10.3f seconds.\n", wtime);
    }

    if(id == 0 && print == 1) {
        //printf("Table after ASP:\n");
        print_tab(tab, n);
        printf("Total distance: %.0lf\n", total);
        printf("Diameter: %d\n", diameter);
    }

    // Clean up
    free_tab(tab, n);
    free(process_start);
    free(process_lines);

    // Shut down MPI.
    ierr = MPI_Finalize();

    return 0;
}
