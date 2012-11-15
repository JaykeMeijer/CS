#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mpi.h"
#include "../table.h"

int do_diam(int **tab, int n)
{
     return 0;
}

void usage() {
        printf ("Run the diameter program with the following parameters. \n");
        printf (" -read filename :: reads the graph from a file.\n");
        printf (" -random N 0/1 :: generates a NxN graph, randomly. \n");
        printf ("               :: if 1, the graph is oriented, otherwise it is not oriented\n");
        return ;
}


int main ( int argc, char *argv[] ) {
    int id;
    int ierr;
    int p=2;
    double wtime;


    int n,m, bad_edges=0, oriented=0, lb, ub, i;
    int **tab;
    int print = 0;
    char FILENAME[100];

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
    }

    if ( id == 0 ) {
        wtime = MPI_Wtime ( );

        printf ( "\n" );
        printf ( "HELLO_MPI - Master process:\n" );
        printf ( "  C/MPI version\n" );
        printf ( "  An MPI example program.\n" );
        printf ( "\n" );
        printf ( "  The number of processes is %d.\n", p );
        printf ( "\n" );
    }

    MPI_Barrier(MPI_COMM_WORLD);
    /*
      Every process prints a hello.
    */
    printf ( " %d says: 'Will work on graph!'\n", id);
    /*
      Process 0 says goodbye.
    */
    if ( id == 0 )
    {
        printf ( "\n" );
        printf ( "HELLO_MPI - Master process:\n" );
        printf ( "  Normal end of execution: 'Goodbye, world!'\n" );

        wtime = MPI_Wtime ( ) - wtime;
        printf ( "\n" );
        printf ( "  Elapsed wall clock time = %f seconds.\n", wtime );
    }
    /*
      Shut down MPI.
    */
    ierr = MPI_Finalize ( );

    return 0;
}
