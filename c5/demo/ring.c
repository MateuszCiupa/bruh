#include<stdio.h>
#include<mpi.h>
#include<unistd.h>

int main () {
    MPI_Init(NULL, NULL);
    int token;
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_rank != 0) {
        MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",
               world_rank, token, world_rank - 1);
        sleep(5);
    } else {
        // Set the token's value if you are process 0
        token = -1;
    }
    
    MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size,
             0, MPI_COMM_WORLD);
    printf("sent!\n");
    // Now process 0 can receive from the last process.
    if (world_rank == 0) {
        MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0,
                 MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process %d received token %d from process %d\n",
               world_rank, token, world_size - 1);
    }
    MPI_Finalize();
}
