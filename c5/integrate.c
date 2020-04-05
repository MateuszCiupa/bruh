#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>

double fun(double x) {
    //return pow(x, 3) + pow(x, 2) + x + 7;
    return pow(x, 2);
}

double integrate(double (*func)(double), double begin, double end, int num_points);

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    double begin = argc > 1 ? atof(argv[1]) : 0;
    double end = argc > 2 ? atof(argv[2]) : 2;
    int num_points = argc > 3 ? atoi(argv[3]) : 10000;

    double results[world_size-1];
    int modulo = num_points % world_size;
    int equal_share = (num_points - modulo) / world_size;
    int n = equal_share + (world_rank < modulo ? 1 : 0);
    double x = (end-begin) / num_points;
    double b = (world_rank < modulo ? equal_share*world_rank : (equal_share+1)*modulo + (world_rank-modulo)*equal_share) * x;
    double e = b + x*n;
    printf("Process %d, begin: %f, end: %f, points: %d\n", world_rank, b, e, n);
    
    if (world_rank == 0) {
        double result = integrate(fun, b, e, n);
        for (int i=1; i<world_size; i++) 
            MPI_Recv(&results[i-1], 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i=0; i<world_size; i++) result += results[i];
        printf("Result is: %f\n", result);
    } else {
        results[world_rank-1] = integrate(fun, b, e, n);
        printf("Proces %d result: %f\n", world_rank, results[world_rank-1]);
        MPI_Send(&results[world_rank-1], 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
}

// metoda prostokątów
double integrate(double (*func)(double), double begin, double end, int num_points) {
    double result = 0;
    for (int i=1; i<=num_points; i++) 
        result += func(begin+i*(end-begin)/num_points) * (end-begin)/num_points;
    return result;
}
