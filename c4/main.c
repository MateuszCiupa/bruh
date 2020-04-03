#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>

int main(int argc, char *argv[]) {
  int N = argc > 1 ? atoi(argv[1]) : 10000;
  int l = argc > 2 ? atoi(argv[2]) : 2;
  int S = (int)sqrt(N);
  int a[S+1], pierwsze[N/10];
  long i, k;
  long int lpodz, llpier = 0;
  double czas;
  clock_t time = clock();
  FILE *fp;

  for (i=2; i<=S; i++)
    a[i] = 1;

  for (i=2; i<=S; i++)
    if (a[i] == 1) {
      pierwsze[llpier++] = i;
      for (k=2*i; k<=S; k+=i)
        a[k] = 0;
    }

  lpodz = llpier;

  omp_set_num_threads(l);

  #pragma omp parallel shared(S, N, pierwsze, lpodz, llpier)
  #pragma omp for nowait
  for (int liczba=S+1; liczba<=N; liczba++) {
    int reszta;
    for (k = 0; k < lpodz; k++) {
      reszta = (liczba % pierwsze[k]);
      if (reszta == 0) break;
    }

    if (reszta != 0) {
      #pragma omp critical
      pierwsze[llpier++] = liczba;
    }
  }

  if ((fp = fopen("primes.txt", "w")) == NULL) {
    printf("Nie moge otworzyc pliku do zapisu\n");
    exit(1);
  }

  for (i = 0; i < llpier; i++)
    fprintf(fp, "%d ", pierwsze[i]);

  fclose(fp);
  time = clock() - time;
  printf("Czas wykonania programu: %f \n", ((double)time) / CLOCKS_PER_SEC);

  return 0;
}
