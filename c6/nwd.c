#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

int generateRandom(int r, int nop)
{
   srand(time(0) + r * nop);
   int n = rand() % 100 + 1;
   return n;
}

int gcd(int a, int b)
{
   while (a != b)
      if (a > b)
         a = a - b;
      else
         b = b - a;
   return a;
}

int main(int argc, char *argv[])
{
   int rank;
   MPI_Comm onedim;
   int number_of_processes;
   int periods = 1, reorder = 0;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
   MPI_Cart_create(MPI_COMM_WORLD, 1, &number_of_processes, &periods, reorder, &onedim);
   MPI_Comm_rank(onedim, &rank);

   int number;
   number = generateRandom(rank, number_of_processes);
   int in;
   int out = number;

   printf("Process: %d Number: %d\n", rank, number);
   int src, dest;
   for (int i = 0; i < log2(number_of_processes); i++)
   {
      MPI_Cart_shift(onedim, 0, 1, &src, &dest);
      MPI_Send(&out, 1, MPI_INT, dest, 0, onedim);
      MPI_Recv(&in, 1, MPI_INT, src, 0, onedim, MPI_STATUS_IGNORE);
      out = gcd(in, out);
   }
   if (rank == 0)
   {
      printf("GCD: %d\n", out);
   }
   MPI_Finalize();
   return 0;
}
