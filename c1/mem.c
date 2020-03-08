#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define PROC_NO 5

int main(int argc, char **argv) {
// create shared memory
	int shmid_result, shmid_range;

	if ((shmid_result = shmget(9876, PROC_NO*sizeof(double), IPC_CREAT | 0666)) == -1) {
		perror("shmget: shmget failed [result]\n");
		exit(1);
	}

	if ((shmid_range = shmget(9875, (PROC_NO+1)*sizeof(int), IPC_CREAT | 0666)) == -1) {
		perror("shmget: shmget failed [range]\n");
		exit(1);
	}

    return 0;
}