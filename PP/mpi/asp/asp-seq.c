/*
 * asp.c:
 *     All-pairs shortest path implementation based on Floyd's
 *     algorithms.
 *
 *      Sequential version.
 *
 * Written by Rob van Nieuwpoort, 6-oct-2003
 * Modified by Jayke Meijer, 15-nov-2012
 */

#include <string.h>
#include <sys/time.h>
#include "../table.h"

/******************** ASP stuff *************************/

void do_asp(int **tab, int n)
{
    int i, j, k, tmp;

    for (k = 0; k < n; k++) {
        for (i = 0; i < n; i++) {
            if (i != k) {
                for (j = 0; j < n; j++) {
                    tmp = tab[i][k] + tab[k][j];
                    if (tmp < tab[i][j]) {
                        tab[i][j] = tmp;
                    }
                }
            }
        }
    }
}

/******************** Main program *************************/

void usage() {
    printf ("Run the asp program with the following parameters. \n");
    printf (" -read filename :: reads the graph from a file.\n");
    printf (" -random N 0/1 :: generates a NxN graph, randomly. \n");
    printf ("               :: if 1, the graph is oriented, otherwise it is not"
        " oriented\n");
    return ;
}

int main(int argc, char **argv)
{
    int n,m, bad_edges=0, oriented=0, lb, ub, i, total, diameter;
    int **tab;
    int print = 0;
    struct timeval start;
    struct timeval end;
    double time;
    char FILENAME[100];


    usage();

    n = 0;
    for(i=1; i<argc; i++) {
        if(!strcmp(argv[i], "-print")) {
            print = 1;
        } else if (!strcmp(argv[i], "-read")) {
            strcpy(FILENAME, argv[i+1]); i++;
        }
        else if (!strcmp(argv[i], "-random")) {
            n = atoi(argv[i+1]); 
            oriented = atoi(argv[i+2]); i+=2;
        }
        else
        {
            n = 4000;
            oriented = 1;
        }
    }
    if (n>0) {
        init_tab(n,&m,&tab,oriented); // last one = oriented or not ...
    }
    else bad_edges = read_tab(FILENAME, &n, &m, &tab, &oriented); 

    fprintf(stderr, "Running ASP with %d rows and %d edges (%d are bad)\n", n,
        m, bad_edges);

    lb = 0;    /* lower bound for rows to be computed */
    ub = n;    /* upper bound for rows to be computed */

//    init_tab(n, &tab);

    if(gettimeofday(&start, 0) != 0) {
        fprintf(stderr, "could not do timing\n");
        exit(1);
    }

    do_asp(tab, n);

    if(gettimeofday(&end, 0) != 0) {
        fprintf(stderr, "could not do timing\n");
        exit(1);
    }

    time = (end.tv_sec + (end.tv_usec / 1000000.0)) - 
        (start.tv_sec + (start.tv_usec / 1000000.0));

    fprintf(stderr, "ASP took %10.3f seconds\n", time);

    if(print == 1) {
        print_tab(tab, n);
    }

    free_tab(tab, n);

    return 0;
}
