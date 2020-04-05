#include<stdio.h>
#include<stdlib.h>

double fun(double x) {
    return x*x;
}

double integrate(double (*func)(double), double begin, double end, int num_points);

int main(int argc, char *argv[]) {
    double b = argc > 1 ? atof(argv[1]) : 0;
    double e = argc > 2 ? atof(argv[2]) : 2;
    int n = argc > 3 ? atoi(argv[3]) : 10000;
    double result = 0;
    double x = (e-b)/n;

    for (int i=1; i<=n; i++) result += fun(b+i*x) * x;

    printf("Result is: %f\n", result);

    return 0;
}
