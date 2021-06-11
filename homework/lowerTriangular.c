#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define N 12
#define M 3

void rand_init_matrix(int *matrix, int num_row, int num_col) {
    for(int i = 0; i < num_row * num_col; i++)
        matrix[i] = rand() % 100;
}

void print_matrix(int *matrix, int num_row, int num_col) {
    printf("\n");
    int p = 0;
    for(int i = 0; i < num_row; i++) {
        for(int j = 0; j < num_col; j++)
            printf("%2d ", matrix[p++]);
        printf("\n");
    }
}

void create_subm_datatype(MPI_Datatype *subm) {
    int lens[3] = {1, 2, 3};
    int disp[3] = {0, N, 2*N};
    MPI_Type_indexed(3, lens, disp, MPI_INT, subm);
}

void create_datatype(MPI_Datatype *subm, MPI_Datatype *new) {
    MPI_Type_commit(subm);
    int lens[2] = {1, 1}, int_size;
    MPI_Type_size(MPI_INT, &int_size);
    MPI_Aint disp[2] = {0, M*(N+1)*int_size};
    MPI_Datatype type[2] = {*subm, MPI_UB};
    MPI_Type_struct(2, lens, disp, type, new);
    MPI_Type_commit(new);
}

int main() {
    int rank;
	int size;
    int a[N][N] = {0};
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype new, subm;
    create_subm_datatype(&subm);
    create_datatype(&subm, &new);
    if(rank == 0) {
        rand_init_matrix(&a[0][0], N, N);
        MPI_Send(&a[0][0], 2, new, 1, 0, MPI_COMM_WORLD);
        print_matrix(&a[0][0], N, N);
    } else {
        MPI_Recv(&a[0][0], 2, new, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        print_matrix(&a[0][0], N, N);
    }
    MPI_Type_free(&new);
    MPI_Type_free(&subm);
	MPI_Finalize();
	return 0;
}