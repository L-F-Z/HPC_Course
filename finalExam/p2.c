// mpicc p2.c -o p2
// mpirun -n 2 ./p2

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

void create_datatype(MPI_Datatype *subm, MPI_Datatype *new) {
    MPI_Type_vector(M, M, N, MPI_INT, subm);
    MPI_Type_commit(subm);
    int lens[2] = {1, 1}, int_size;
    MPI_Type_size(MPI_INT, &int_size);
    MPI_Aint disp[2] = {0, 2*M*N*int_size};
    MPI_Datatype type[2] = {*subm, MPI_UB};
    MPI_Type_struct(2, lens, disp, type, new);
    MPI_Type_commit(new);
}

void create_datatype2(MPI_Datatype *new2) {
    int lens[2*M];
    int disp[2*M];
    for(int i = 0; i < 2*M; i++)
        lens[i] = M;
    for(int i = 0; i < M; i++)
        disp[i] = i * N;
    for(int i = 0; i < M; i++)
        disp[M+i] = (2 * M + i) * N;    
    MPI_Type_indexed(2*M, lens, disp, MPI_INT, new2);
    MPI_Type_commit(new2);
}

int main() {
    int rank;
	int size;
    int a[N][N] = {0};
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Datatype new, new2, subm;
    create_datatype(&subm, &new);
    create_datatype2(&new2);
    if(rank == 0) {
        rand_init_matrix(&a[0][0], N, N);
        MPI_Send(&a[0][0], 1, new2, 1, 0, MPI_COMM_WORLD);
        print_matrix(&a[0][0], N, N);
    } else {
        MPI_Recv(&a[0][0], 1, new2, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        print_matrix(&a[0][0], N, N);
    }
    MPI_Type_free(&new);
    MPI_Type_free(&subm);
    MPI_Type_free(&new2);
	MPI_Finalize();
	return 0;
}