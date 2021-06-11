// mpicc p3.c -o p3
// mpirun -n 2 ./p3

#include <stdio.h>
#include "mpi.h"

typedef struct {
    int m[3];
    float a[2];
    char c[5];
} datatype;

void create_datatype(MPI_Datatype *new) {
    int lens[3] = {3, 2, 5};
    MPI_Aint disp[3];
    MPI_Datatype type[3] = {MPI_INT, MPI_FLOAT, MPI_CHAR};
    datatype tmp;
    MPI_Address(&tmp.m, &disp[0]);
    MPI_Address(&tmp.a, &disp[1]);
    MPI_Address(&tmp.c, &disp[2]);
    for(int i = 2; i >= 0; i--)
        disp[i] -= disp[0];
    MPI_Type_struct(3, lens, disp, type, new);
}

int main() {
    int rank;
	int size;
    datatype x[10] = {0};
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype new;
    create_datatype(&new);
    MPI_Type_commit(&new);
    if(rank == 0) {
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 3; j++)
                x[i].m[j] = 3 + i;
            for(int j = 0; j < 2; j++)
                x[i].a[j] = 2.0 + i;
            for(int j = 0; j < 5; j++)
                x[i].c[j] = 'a' + i;
        }
        MPI_Send(x, 5, new, 1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(x, 5, new, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < 10; i++) {
            for(int j = 0; j < 3; j++)
                printf("%2d ", x[i].m[j]);
            for(int j = 0; j < 2; j++)
                printf("%5.2f ", x[i].a[j]);
            for(int j = 0; j < 5; j++)
                printf("%c ", x[i].c[j]);
            printf("\n");
        }
    }
    MPI_Type_free(&new);
	MPI_Finalize();
	return 0;
}