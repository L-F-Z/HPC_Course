// mpicc p4.c -o p4
// mpirun -n 3 ./p4

#include "mpi.h"
#include <stdio.h>
#include <string.h>

void MY_Allgather(
    void *sendbuf,
    int sendcount,
    MPI_Datatype sendtype,
    void *recvbuf,
    int recvcount,
    MPI_Datatype recvtype,
    MPI_Comm comm
) {
    if(sendcount != recvcount || sendtype != recvtype)
        return;
    int elem_size, rank, world_size;
    MPI_Type_size(sendtype, &elem_size);
    MPI_Comm_size(comm, &world_size);
    MPI_Comm_rank(comm, &rank);
    int ssize = sendcount * elem_size;
    
    for(int i = 0; i < world_size; i++) {
        if(i == rank) {
            for(int j = 0; j < world_size; j++) {
                if(j == i)
                    memcpy(recvbuf+ssize*j, sendbuf, ssize);
                else
                    MPI_Send(sendbuf, sendcount, sendtype, j, i, comm);
            }
        } else {
            MPI_Recv(recvbuf+ssize*i, recvcount, recvtype, i, i, comm, MPI_STATUS_IGNORE);
        }
    }
}

int main() {
	int rank;
	int size;
    int send_data[2], recv_data[6];
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    for(int i = 0; i < 2; i++)
        send_data[i] = rank * 2 + i;
    MY_Allgather(send_data, 2, MPI_INT, recv_data, 2, MPI_INT, MPI_COMM_WORLD);
	printf("[%d / %d] %2d %2d %2d %2d %2d %2d\n", rank, size, 
           recv_data[0], recv_data[1], recv_data[2], recv_data[3], recv_data[4], recv_data[5]);
	MPI_Finalize();
	return 0;
}