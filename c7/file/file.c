#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_data(char *fname, int *nwords, char *words, int *indexes, int log_data)
{
    FILE *f = fopen(fname, "r");

    if (!f)
    {
        perror("Problem while reading file");
        exit(EXIT_FAILURE);
    }

    //code

    fclose(f);
}