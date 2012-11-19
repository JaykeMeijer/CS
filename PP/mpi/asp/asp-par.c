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
double do_dist(int **tab, int n, int myid, int no_proc)
{
    int i, j;
    double total = 0;
    int lines = n / no_proc;
    int start = myid * lines;
    double received_total;
    MPI_Status stat;

    printf("Going to work. My id=%d, no_proc=%d, n=%d\n", myid, no_proc, n);
    printf("Lines=%d, start=%d\n", lines, start);

    // Calculate total for my part of the table
    for(i = start; i < start + lines; i++)
        for(j = 0; j < n; j++)
            if(tab[i][j] != MAX_DISTANCE)
                total += tab[i][j];

    printf("Done calculating. Total of proc %d is %.0lf\n", myid, total);

    if(myid != 0) {
        // Slave process, send to root.
        printf("Process %d sending total of %.0lf to master\n", myid, total);
        MPI_Send(&total, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else {
        // Root, receive all slave data
        for(i = 1; i < no_proc; i++) {
            MPI_Recv(&received_total, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &stat);
            printf("Received from %d a total of %.0lf\n", i, received_total);
            total += received_total;
        }
    }

    return total;
}

/******************** ASP calculation *************************/
void do_asp(int **tab, int n, int myid, int no_proc)
{
    int i, j, k, tmp;
    int lines = n / no_proc;
    int start = myid * lines;
    int other_start;
    MPI_Status stat;

    // Run the ASP algorithm. Update every pass.
    for (k = 0; k < n; k++) {
        // Perform a calculation pass over my part of the table
        for (i = start; i < start + lines; i++) {
            if (i != k) {
                for (j = 0; j < n; j++) {
                    tmp = tab[i][k] + tab[k][j];
                    if (tmp < tab[i][j])
                        tab[i][j] = tmp;
                }
            }
        }

        // Send the just calculated data to the other processes
        for(i = 0; i < no_proc; i++) {
            if(i == myid) {
                // My turn to send
                for(j = start; j < start + lines; j++)
                    MPI_Bcast(tab[j], n, MPI_INT, 0, MPI_COMM_WORLD);
            } else {
                // Listen and update
                other_start = i * lines;
                for(j = other_start; j < other_start + lines; j++) {
                    MPI_Recv(tab[j], n, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
                }
            }
        }
    }
}

/******************** Diam calculation *************************/
int do_diam(int **tab, int n, int myid, int no_proc)
{
    int i, j, longest = 0;
    int lines = n / no_proc;
    int start = myid * lines;
    int received_longest;
    MPI_Status stat;

    // Find the longest path in my part of the table
    for(i = start; i < start + lines; i++)
        for(j = 0; j < n; j++)
            if(tab[i][j] != 0 && tab[i][j] > longest)
                longest = tab[i][j];

    printf("Done searching. Longest of proc %d is %d\n", myid, longest);
    if(myid != 0) {
        // Slave process, send to root.
        printf("Process %d sending total of %d to master\n", myid, longest);
        MPI_Send(&longest, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else {
        // Root, receive all slave data
        for(i = 1; i < no_proc; i++) {
            MPI_Recv(&received_longest, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
            printf("Received from %d a longest of %d\n", i, received_longest);
            if(received_longest > longest)
                longest = received_longest;
        }
    }

    return longest;
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


    int n,m, bad_edges=0, oriented=0, i, diameter;
    int **tab;
    int print = 0;
    char FILENAME[100];
    MPI_Status stat;

    /*MPI stuff*/

    /*
     *   Initialize MPI.
     *   */
    ierr = MPI_Init ( &argc, &argv );
    if(ierr != MPI_SUCCESS) {
            perror("Error with initializing MPI");
            exit(1);
    }

    /*
     *   Get the number of processes.
     *   */
    ierr = MPI_Comm_size ( MPI_COMM_WORLD, &p );
    /*
     *   Get the individual process ID.
     *   */
    ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &id );
    /*
     *   Process 0 reads data + prints welcome
     *   */

    if (id==0) {
        usage();

        n = 0;
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

        printf("The number of processes is %d.\nNow distributing table", p);

        // Distribute the table to all machines.
        for(i = 0; i < n; i++)
            MPI_Bcast(tab[i], n, MPI_INT, 0, MPI_COMM_WORLD);

        printf("Table is distributed\n");
    } else {
        // Receive the table
        for(i = 0; i < n; i++)
            MPI_Recv(tab[i], n, MPI_INT, i, 0, MPI_COMM_WORLD, &stat);
    }

    /******************** Distance calculation *************************/
    if(id == 0)
        wtime = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    total = do_dist(tab, n, id, p);

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        fprintf (stderr, "Distance took %f seconds.\n", wtime);
    }

    /******************** ASP *************************/
    if(id == 0)
        wtime = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    do_asp(tab, n, id, p);

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        fprintf (stderr, "ASP took %f seconds.\n", wtime);
    }

    /******************** Diameter *************************/
    if(id == 0)
        wtime = MPI_Wtime();

    MPI_Barrier(MPI_COMM_WORLD);
    diameter = do_diam(tab, n, id, p);

    if(id == 0) {
        wtime = MPI_Wtime() - wtime;
        fprintf (stderr, "Diameter took %f seconds.\n", wtime);
    }

    if(id == 0 && print == 1) {
        printf("Table after ASP:\n");
        print_tab(tab, n);
        printf("Total distance: %.0lf\n", total);
        printf("Diameter: %d\n", diameter);
    }


    /*
      Shut down MPI.
    */
    ierr = MPI_Finalize ( );

    return 0;
}
