#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <string.h>
#include <time.h>

#define BUFFOR_SIZE 80

double sum(double *vector, int a, int b) {
	int i;
	double sum = 0.0f;
	for (i=a; i<b; i++) sum += vector[i];
	return sum;
}

char *concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

void on_usr1(int sig) {
	printf("Usr1 signal [%d]\n", getpid());
}

int main(int argc, char **argv) {
// vector size
	char *filename = argc > 1 ? argv[1] : "vector.dat";
	int proc_no = argc > 2 ? atoi(argv[2]) : 5;
	FILE *f = fopen("vector.dat", "r");
	char buffor[BUFFOR_SIZE+1];
	fgets(buffor, BUFFOR_SIZE, f);
	int n = atoi(buffor);

// obtain access to shared memory segments
	int shmid_result, shmid_range, shmid_vector;
	double *result, *vector;
	int *range;

	if ((shmid_result = shmget(9876, proc_no*sizeof(double), 0666 | IPC_CREAT)) == -1) {
		perror("shmget: shmget failed [result]\n");
		exit(1);
	}

	if ((shmid_range = shmget(9875, (proc_no+1)*sizeof(int), 0666 | IPC_CREAT)) == -1) {
		perror("shmget: shmget failed [range]\n");
		exit(1);
	}

	if ((shmid_vector = shmget(9874, n*sizeof(double), 0666 | IPC_CREAT)) == -1) {
		perror("shmget: shmget failed [vector]\n");
		exit(1);
	}

// attach shared memory segments
	if ((result = shmat(shmid_result, NULL, 0)) == (double *) -1) {
		perror("shmat: shmat failed [result]\n");
		exit(1);
	}

	if ((range = shmat(shmid_range, NULL, 0)) == (int *) -1) {
		perror("shmat: shmat failed [range]\n");
		exit(1);
	}

	if ((vector = shmat(shmid_vector, NULL, 0)) == (double *) -1) {
		perror("shmat: shmat failed [vector]\n");
		exit(1);
	}

// set USR1 signal action
	sigset_t mask;
	sigemptyset(&mask);
	struct sigaction usr1;
	usr1.sa_mask = mask;
	usr1.sa_flags = 0;
	usr1.sa_handler = (&on_usr1);
	sigaction(SIGUSR1, &usr1, NULL);

// what the fork
	int i;
	pid_t children[proc_no];
	pid_t pid;
	for (i=0; i<proc_no; i++)
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

// read vector from file
	for (i=0; i<n; i++) {
		fgets(buffor, BUFFOR_SIZE, f);
		vector[i] = atof(buffor);
	}

	fclose(f);

// set range for each child process
	range[0] = 0;
	for (i=1; i<proc_no; i++) range[i] = (int)(n / proc_no) * i;
	range[proc_no] = n - 1;

	printf("Range vector: [ ");
	for (i=0; i<=proc_no; i++) printf("%d ", range[i]);
	printf("]\n");


// send SIGUSR1 to children
	sleep(1);
	for (i=0; i<proc_no; i++) kill(children[i], SIGUSR1);

	clock_t time = clock();
// wait for damn children
	for (i=0; i<proc_no; i++)
		if (wait(0) == -1) {
			perror("wait: wait failed [-1 dawg]");
			exit(1);
		}
	time = clock() - time;

// sum result + the last value in the vector
	printf("\n\n");
	printf("THE RESULT IS: %f\n", sum(result, 0, proc_no) + vector[n-1]);
	printf("TIME TAKEN: %f\n", ((double)time)/CLOCKS_PER_SEC);

// cleanup
	shmdt(vector);
	shmdt(range);
	shmdt(result);

	shmctl(shmid_result, IPC_RMID, NULL);
	shmctl(shmid_vector, IPC_RMID, NULL);
	shmctl(shmid_range, IPC_RMID, NULL);

	return 0;
}