#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>

double global = 0;
double frobenius = 0;
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct margs {
    double **A, **B, **C;
    int n, row, col;
} margs_t;

void *thread_function(void *arg) {
    margs_t *margs = (margs_t *)arg;
    int i;
    double s = 0;
    for (i=0; i<margs->n; i++)
        s += margs->A[margs->row][i] * margs->B[i][margs->col];
    margs->C[margs->row][margs->col] = s;
    pthread_mutex_lock(&mymutex);
    global += s;
    frobenius += s*s;
    pthread_mutex_unlock(&mymutex);
}

void print_matrix(double**A, int m, int n) {
    int i, j;
    printf("[");
    for (i=0; i<m; i++) {
        for (j=0; j<n; j++) printf("%f ", A[i][j]);
        printf("\n");
    }
    printf("]\n");
}

int main(int argc, char *argv[]) {
// wczytanie macierzy z pliku oraz inicjalizacja macierzy wynikowej
    FILE *fpa, *fpb;
    double **A, **B, **C;
    int ma, na, mb, nb, i, j;
    double x;

    fpa = fopen("A.txt", "r");
    fpb = fopen("B.txt", "r");

    if (fpa == NULL || fpb == NULL) {
        perror("błąd otwarcia pliku");
        exit(-10);
    }

    fscanf(fpa, "%d", &ma);
    fscanf (fpa, "%d", &na);

    fscanf (fpb, "%d", &mb);
    fscanf (fpb, "%d", &nb);

    printf("pierwsza macierz ma wymiar %d x %d, a druga %d x %d\n", ma, na, mb, nb);

    if (na != mb) {
        printf("Złe wymiary macierzy!\n");
        return EXIT_FAILURE;
    }
    
    /*Alokacja pamięci*/
    A = malloc(ma*sizeof(double));
    for (i=0; i<ma; i++) A[i] = malloc(na*sizeof(double));

    B = malloc(mb*sizeof(double));
    for (i=0; i<mb; i++) B[i] = malloc(nb*sizeof(double));

    /*Macierz na wynik*/
    C = malloc(ma*sizeof(double));
    for (i=0; i<ma; i++) C[i] = malloc(nb*sizeof(double));

    //printf("Rozmiar C: %dx%d\n", ma, nb);
    for (i=0; i<ma; i++) 
        for (j=0; j<na; j++) {
            fscanf(fpa, "%lf", &x);
            A[i][j] = x;
        }

    //printf("A:\n");
    //print_matrix(A, ma, na);

    for (i=0; i<mb; i++)
        for (j=0; j<nb; j++) {
            fscanf(fpb, "%lf", &x);
            B[i][j] = x;
        }

    //printf("B:\n");
    //print_matrix(B, mb, nb);

    pthread_t mythread[ma][nb];
    margs_t margs[ma][nb];
    for (i=0; i<ma; i++)
        for (j=0; j<nb; j++) {
            margs[i][j].A = A;
            margs[i][j].B = B;
            margs[i][j].C = C;
            margs[i][j].n = na;
            margs[i][j].row = i;
            margs[i][j].col = j;
            if (pthread_create(&(mythread[i][j]), NULL, thread_function, &(margs[i][j]))) {
                printf("error creating thread");
                abort();
            }
            
        }

    for (i=0; i<ma; i++) 
        for (j=0; j<nb; j++)
            if (pthread_join(mythread[i][j], NULL)) {
                printf("error joining thread");
                abort();
            }

    print_matrix(C, ma, nb);
    printf("Suma elementów macierzy wynosi: %f\n", global);
    printf("Norma Frobeniusa wynosi: %f\n", sqrt(frobenius));
    
    for (i=0; i<ma; i++) free(A[i]);
    free(A);
    for (i=0; i<mb; i++) free(B[i]);
    free(B);
    for (i=0; i<ma; i++) free(C[i]);
    free(C);
    return 0;
}
