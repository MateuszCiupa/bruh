#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>

#define BUFFOR_SIZE 80
#define PROC_NO 5

double sum(double *vector, int a, int b) {
	int i;
	double sum = 0.0f;
	for (i=a; i<b; i++) sum += vector[i];
	return sum;
}

void on_usr1(int sig) {
	printf("Usr1 signal [%d]\n", getpid());
}

int main(int argc, char **argv) {
// obtain access to shared memory segments
	int shmid_result, shmid_range;
	double *result;
	int *range;

	if ((shmid_result = shmget(9876, PROC_NO*sizeof(double), 0666)) == -1) {
		perror("shmget: shmget failed [result]\n");
		exit(1);
	}

	if ((shmid_range = shmget(9875, (PROC_NO+1)*sizeof(int), 0666)) == -1) {
		perror("shmget: shmget failed [range]\n");
		exit(1);
	}

// attach shared memory segments
	if ((result = shmat(shmid_result, NULL, 0)) == (char *) -1) {
		perror("shmat: shmat failed [result]\n");
		exit(1);
	}

	if ((range = shmat(shmid_range, NULL, 0)) == (char *) -1) {
		perror("shmat: shmat failed [range]\n");
		exit(1);
	}

// read vector from file
	FILE *f = fopen("vector.dat", "r");
    char buffor[BUFFOR_SIZE+1];
	double *vector;
	int n, i;

	fgets(buffor, BUFFOR_SIZE, f);
 	n = atoi(buffor);
	vector = malloc(sizeof(double) * n);

	for (i=0; i<n; i++) {
		fgets(buffor, BUFFOR_SIZE, f);
		vector[i] = atof(buffor);
	}

	fclose(f);

// set range for each child process
	range[0] = 0;
	for (i=1; i<PROC_NO; i++) range[i] = (int)(n / PROC_NO) * i;
	range[PROC_NO] = n - 1;

	printf("Range vector: [ ");
	for (i=0; i<=PROC_NO; i++) printf("%d ", range[i]);
	printf("]\n");

// set USR1 signal action
	sigset_t mask;
	sigemptyset(&mask);
	struct sigaction usr1;
	usr1.sa_mask = mask;
	usr1.sa_flags = 0;
	usr1.sa_handler = (&on_usr1);
	sigaction(SIGUSR1, &usr1, NULL);

// what the fork
	pid_t children[PROC_NO];
	pid_t pid;
	for (i=0; i<PROC_NO; i++)
		switch (pid = fork()) {
			case -1:
				perror("fork: fork failed [-1 dawg]");
				exit(1);
			case 0:
				printf("Process nr. %d with PID %d paused.\n", i, getpid());
				pause();
				result[i] = sum(vector, range[i], range[i+1]);
				printf("Result of process nr. %d is %f\n", i, result[i]);
				return 0;
			default:
				children[i] = pid;
		}

// send SIGUSR1 to children
	sleep(1);
	for (i=0; i<PROC_NO; i++) kill(children[i], SIGUSR1);

// wait for damn children
	for (i=0; i<PROC_NO; i++)
		if (wait(0) == -1) {
			perror("wait: wait failed [-1 dawg]");
			exit(1);
		}

// sum result + the last value in the vector
	printf("THE RESULT IS: %f\n", sum(result, 0, PROC_NO) + vector[n-1]);

	free(vector);
	return 0;
}