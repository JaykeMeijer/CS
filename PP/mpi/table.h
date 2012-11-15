#include <stdio.h>
#include <stdlib.h>

#define MAX_DISTANCE 256
// #define VERBOSE

void init_tab(int n, int *mptr, int ***tabptr, int oriented);
int read_tab(char *INPUTFILE, int *nptr, int *mptr, int ***tabptr, int *optr);
void free_tab(int **tab, int n);
void print_tab(int **tab, int n);
