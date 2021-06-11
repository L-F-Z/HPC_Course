#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int iam, np;
    MPI_Comm comm;

    /* Start MPI environment */
    MPI_Init(&argc, &argv);
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    MPI_Comm_rank(comm, &iam);
    MPI_Comm_size(comm, &np);

    /* MAIN work here */
    printf("\nThe process %d of %d is running!\n", iam, np);

    /* finished, terminate the MPI */
    MPI_Finalize();
}