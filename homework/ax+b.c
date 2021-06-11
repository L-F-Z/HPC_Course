#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define N 16
#define M 4
#define P 4 

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

void split_matrix(int *src, int *dst, int num_row, int num_col, int np, int p) {
    int new_col = num_col / np;
    for(int i = 0; i < num_row; i++) {
        int ptr = i * num_col + p * new_col;
        for(int j = 0; j < new_col; j++)
            dst[i*new_col+j] = src[ptr+j];
    }
}

int main() {
    int rank;
	int size;
    int a[N][M], b[M], x[M], y[N], reduced_y[N];
    int na[N][N], nx[N], nb[N];
    MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(rank == 0) {
        /* Random Init */
        rand_init_matrix(&na[0][0], N, N);
        // print_matrix(&na[0][0], N, N);
        for(int i = 0; i < N; i++) {
            nx[i] = rand() % 100;
            nb[i] = rand() % 100;
        }

        /* Distribute Data */
        for(int i = 1; i < P; i++) {
            split_matrix(&na[0][0], &a[0][0], N, N, P, i);
            MPI_Send(&a[0][0], N*M, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        split_matrix(&na[0][0], &a[0][0], N, N, P, 0);
        // print_matrix(&a[0][0], N, M);
        for(int i = 1; i < P; i++)
            MPI_Send(&nx[i*M], M, MPI_INT, i, 0, MPI_COMM_WORLD);
        for(int i = 0; i < M; i++)
            x[i] = nx[i];
    } else {
        MPI_Recv(&a[0][0], N*M, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // print_matrix(&a[0][0], N, M);
        MPI_Recv(&x[0], M, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    /* Calculation */
    for(int i = 0; i < N; i++) {
        y[i] = 0;
        for(int j = 0; j < M; j++)
            y[i] += a[i][j] * x[j];
    }
    MPI_Reduce(&y[0], &reduced_y[0], N, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    if(rank == 0) {
        for(int i = 0; i < N; i++) {
            y[i] = reduced_y[i] + nb[i];
            printf("%5d ", y[i]);
        }
        printf("\n");

        /* Check Result */
        int tmp_y[N] = {0};
        for(int i = 0; i < N; i++)
            for(int j = 0; j < N; j++)
                tmp_y[i] += na[i][j] * nx[j];
        for(int i = 0; i < N; i++) {
            tmp_y[i] += nb[i];
            printf("%5d ", tmp_y[i]);
        }
        printf("\n");
    }
	MPI_Finalize();
    return 0;
}