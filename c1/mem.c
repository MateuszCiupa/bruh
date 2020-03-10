#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv) {
	int size = argc > 1 ? atoi(argv[1]) : 10;
	double a = argc > 2 ? atof(argv[2]) : 0.0f;
	double b = argc > 3 ? atof(argv[3]) : 100.0f;
	char *fname = argc > 4 ? argv[4] : "vector.dat";

	FILE *f = fopen(fname, "w");

	fprintf(f, "%d\n", size);

	int i;
	srand(time(0));
	for (i=0; i<size; i++) {
		double x = a + (((double) rand()) / RAND_MAX) * (b-a);
		fprintf(f, "%f\n", x);
	}

	fclose(f);

    return 0;
}