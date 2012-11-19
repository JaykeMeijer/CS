/*
 * asp-seq.c:
 *     All-pairs shortest path implementation based on Floyd's
 *     algorithms. Also diameter and distance calculation.
 *
 *      Sequential version.
 *
 * Written by Rob van Nieuwpoort, 6-oct-2003
 * Modified by Jayke Meijer, 15-nov-2012
 */

#include <string.h>
#include <sys/time.h>
#include "table.h"

/******************** Distance calculation *************************/
double do_dist(int **tab, int n)
{
    int i, j;
    double total = 0;
    for(i = 0; i < n; i++)
        for(j = 0; j < n; j++)
            if(tab[i][j] != MAX_DISTANCE)
                total += tab[i][j];

    return total;
}

/******************** ASP calculation *************************/
void do_asp(int **tab, int n)
{
    int i, j, k, tmp;

    for (k = 0; k < n; k++)
        for (i = 0; i < n; i++)
            if (i != k)
                for (j = 0; j < n; j++) {
                    tmp = tab[i][k] + tab[k][j];
                    if (tmp < tab[i][j])
                        tab[i][j] = tmp;
                }
}

/******************** Diam calculation *************************/
int do_diam(int **tab, int n)
{
    int i, j, longest = 0;
    for(i = 0; i < n; i++)
        for(j = 0; j < n; j++)
            if(tab[i][j] != 0 && tab[i][j] > longest)
                longest = tab[i][j];

    return longest;
}

/******************** Main program *************************/
void usage() {
    printf ("Run the program with the following parameters. \n");
    printf (" -read filename :: reads the graph from a file.\n");
    printf (" -random N 0/1 :: generates a NxN graph, randomly. \n");
    printf ("               :: if 1, the graph is oriented, otherwise it is not"
        " oriented\n");
    return ;
}

int main(int argc, char **argv)
{
    int n,m, bad_edges=0, oriented=0, i, diameter;
    double total;
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

    fprintf(stderr, "Running program with %d rows and %d edges (%d are bad)\n",
        n, m, bad_edges);

//    init_tab(n, &tab);

    /******************** Distance calculation *************************/
    if(gettimeofday(&start, 0) != 0) {
        fprintf(stderr, "could not do timing\n");
        exit(1);
    }

    total = do_dist(tab, n);

    if(gettimeofday(&end, 0) != 0) {
        fprintf(stderr, "could not do timing\n");
        exit(1);
    }

    time = (end.tv_sec + (end.tv_usec / 1000000.0)) - 
        (start.tv_sec + (start.tv_usec / 1000000.0));

    fprintf(stderr, "Distance calculation took %10.3f seconds\n", time);

    /******************** ASP calculation *************************/
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

    /******************** Diam calculation *************************/
    if(gettimeofday(&start, 0) != 0) {
        fprintf(stderr, "could not do timing\n");
        exit(1);
    }

    diameter = do_diam(tab, n);

    if(gettimeofday(&end, 0) != 0) {
        fprintf(stderr, "could not do timing\n");
        exit(1);
    }

    time = (end.tv_sec + (end.tv_usec / 1000000.0)) - 
        (start.tv_sec + (start.tv_usec / 1000000.0));

    fprintf(stderr, "Diameter calculation took %10.3f seconds\n", time);

    if(print == 1) {
        printf("Table after ASP:\n");
        print_tab(tab, n);
        printf("Total distance: %.0lf\n", total);
        printf("Diameter: %d\n", diameter);
    }

    free_tab(tab, n);

    return 0;
}
