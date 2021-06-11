#include "mpi.h"
#include <stdio.h>
#include <string.h>

void MY_Alltoall(
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
    if(world_size != sendcount)
        return;
    
    for(int i = 0; i < sendcount; i++) {
        if(i == rank) {
            for(int j = 0; j < sendcount; j++) {
                if(j == i)
                    memcpy(recvbuf+elem_size*j, sendbuf+elem_size*j, elem_size);
                else
                    MPI_Send(sendbuf+elem_size*j, 1, sendtype, j, i, comm);
            }
        } else {
            MPI_Recv(recvbuf+elem_size*i, 1, recvtype, i, i, comm, MPI_STATUS_IGNORE);
        }
    }
}

int main() {
	int rank;
	int size;
    int send_data[6], recv_data[6];
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    for(int i = 0; i < 6; i++)
        send_data[i] = rank * 6 + i;
    MY_Alltoall(send_data, 6, MPI_INT, recv_data, 6, MPI_INT, MPI_COMM_WORLD);
	printf("[%d / %d] %2d %2d %2d %2d %2d %2d\n", 
           rank, 
           size, 
           recv_data[0],
           recv_data[1],
           recv_data[2],
           recv_data[3],
           recv_data[4],
           recv_data[5]);
	MPI_Finalize();
	return 0;
}