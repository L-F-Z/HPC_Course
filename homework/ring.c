#include "mpi.h"
#include <stdio.h>

void ring_s_r(
    int m,    // send data
    int iam,  // my id
    int np,   // num of processes
    int comm, // communicator
    int *n    // recv data
) {
    int next = (iam + 1) % np;
    int front = (iam - 1 + np) % np;
    int tag = 1;
    MPI_Status st;

    if (iam == 0) {
        MPI_Send(&m, 1, MPI_INT, next, tag, comm);
        MPI_Recv(n, 1, MPI_INT, front, tag, comm, &st);
    } else {
        MPI_Recv(n, 1, MPI_INT, front, tag, comm, &st);
        MPI_Send(&m, 1, MPI_INT, next, tag, comm);
    }
}

void ring_sr(
    int m,    // send data
    int iam,  // my id
    int np,   // num of processes
    int comm, // communicator
    int *n    // recv data
) {
    int next = (iam + 1) % np;
    int front = (iam - 1 + np) % np;
    int tag = 1;
    MPI_Status st;

    MPI_Sendrecv(&m, 1, MPI_INT, next, tag, n, 1, MPI_INT, front, tag, comm, &st);
}
 
int main() {
	int rank;
	int size;
    int send_data, recv_data;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    send_data = 2 * rank;
    // ring_s_r(send_data, rank, size, MPI_COMM_WORLD, &recv_data);
    ring_sr(send_data, rank, size, MPI_COMM_WORLD, &recv_data);
	printf("[%d / %d] Send %d, Recv %d\n", rank, size, send_data, recv_data);
	MPI_Finalize();
	return 0;
}
