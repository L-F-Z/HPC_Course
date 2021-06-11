#include "mpi.h"
#include <stdio.h>

void mesh(
    int iam,
    int np,
    MPI_Comm comm,
    int p,
    int q,
    int *myrow,
    int *mycol,
    MPI_Comm *rowcomm,
    MPI_Comm *colcomm
) {
    int key = iam;
    int color;
    if (np < p * q)
        return;

    /* row communicator */
    if (iam < p * q)
        color = iam / q;
    else
        color = MPI_UNDEFINED;
    MPI_Comm_split(comm, color, key, rowcomm);

    /* column communicator */
    if (iam < p * q)
        color = iam % q;
    else
        color = MPI_UNDEFINED;
    MPI_Comm_split(comm, color, key, colcomm);

    /* get location */
    if (iam < p * q) {
        MPI_Comm_rank(*colcomm, myrow);
        MPI_Comm_rank(*rowcomm, mycol);
    }
}

void MY_Bcast(
    int *s,
    int myrow,
    int mycol,
    MPI_Comm rowcomm,
    MPI_Comm colcomm
) {
    if(myrow == 0) {
        MPI_Bcast(s, 1, MPI_INT, 0, rowcomm);
        MPI_Bcast(s, 1, MPI_INT, 0, colcomm);
    } else {
        MPI_Bcast(s, 1, MPI_INT, 0, colcomm);
    }
}

int main()
{
	int rank;
	int size;
    int p, q;
    int myrow, mycol;
    MPI_Comm rowcomm, colcomm;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    p = 3, q = 4;
    mesh(rank, size, MPI_COMM_WORLD, p, q, &myrow, &mycol, &rowcomm, &colcomm);

    int s;
    if(rank == 0)
        s = 5;
    MY_Bcast(&s, myrow, mycol, rowcomm, colcomm);

	printf("[%d / %d] Row %d, Col %d, s = %d\n", rank, size, myrow, mycol, s);
	MPI_Finalize();
	return 0;
}