#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

#define VECTOR_SIZE 1000000
#define SHARE_SIZE 1000

int *get_rand_vector(int begin, int end, int len) {
    int *vector = (int *)malloc(len * sizeof(vector));
    if (vector == NULL) return NULL;

    srand(time(0));
    for (int i=0; i<len; i++) {
		int x = begin + (int)((((double) rand()) / RAND_MAX) * (end-begin));
		vector[i] = x;
	}

    return vector;
}

int get_vector_sum(int len, int *vector) {
    int sum = 0;
    for (int i=0; i<len; i++) sum += vector[i];
    return sum;
}

int main(int argc, char *argv[]) {
    int world_size, cart_rank, prev_cart_rank, next_cart_rank, *vector, share[SHARE_SIZE], vector_sum = 0;
   
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    const int dims[] = { world_size }, periods[] = { 0 }, reorder = 1;
    MPI_Comm cart_comm;

    MPI_Cart_create(MPI_COMM_WORLD, 1, dims, periods, reorder, &cart_comm);
    MPI_Comm_rank(cart_comm, &cart_rank);
    MPI_Cart_shift(cart_comm, 0, 1, &prev_cart_rank, &next_cart_rank);

    // init vector for 1st process
    if (cart_rank == 0) {
        vector = get_rand_vector(0, 1000, VECTOR_SIZE);
        printf("Init vector sum: %d\n", get_vector_sum(VECTOR_SIZE, vector));
    }
    
    for (int i=0; i<VECTOR_SIZE/SHARE_SIZE; i++) {
        if (cart_rank == 0) memcpy(share, vector+i*SHARE_SIZE, SHARE_SIZE*sizeof(*share));
            
        // for 1st proc prev_cart_rank == -1
        MPI_Recv(share, SHARE_SIZE, MPI_INT, prev_cart_rank, 0, cart_comm, MPI_STATUS_IGNORE);

        if (cart_rank == world_size - 1) vector_sum += get_vector_sum(SHARE_SIZE, share);

        // for world_size-1 proc next_cart_rank == -1
        MPI_Send(share, SHARE_SIZE, MPI_INT, next_cart_rank, 0, cart_comm);
    }

    if (cart_rank == world_size - 1) printf("Rank %d calculated sum, which is: %d\n", cart_rank, vector_sum);

    MPI_Finalize();
}