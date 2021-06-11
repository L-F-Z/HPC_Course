#include <stdio.h>
#include "mpi.h"

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


void gemm(
    int m,
    int l,
    int n,
    float *a,
    int lda,
    float *b,
    int ldb,
    float *c,
    int ldc
) {
    int i, j, k;
    for(i = 0; i < m; i++)
        for(j = 0; j < n; j++)
            for(k = 0; k < l; k++)
                c[i*ldc+j] += a[i*lda+k] * b[k*ldb+j];
    return;
}

void assign(
    int m,
    int n,
    float *a,
    int lda,
    float *b,
    int ldb
) {
    int i, j;
    for(i = 0; i < m; i++)
        for(j = 0; j < n; j++)
            a[i*lda+j] = b[i*ldb+j];
}

void cannon(
    int m,
    int l,
    int n,
    float *a,
    int lda,
    float *b,
    int ldb,
    float *c,
    int ldc,
    float *work,
    int p,         // num of processes
    MPI_Comm comm
) {
    int iam, np, i, j, k;
    int rowid, colid;
    MPI_Comm rowcom, colcom;
    MPI_Status st;
    MPI_Comm_size(comm, &np);
    MPI_Comm_rank(comm, &iam);
    if(p*p > np) return;
    mesh(iam, np, MPI_COMM_WORLD, p, p, &rowid, &colid, &rowcom, &colcom);

    /* define A's and B's type*/
    MPI_Datatype mta, mtb;
    MPI_Type_vector(m, l, lda, MPI_FLOAT, &mta);
    MPI_Type_vector(l, n, ldb, MPI_FLOAT, &mtb);
    MPI_Type_commit(&mta);
    MPI_Type_commit(&mtb);

    k = rowid;
    int next = (rowid - 1 + p) % p;
    int front = (rowid + 1) % p;
    for(i = 0; i < m; i++)
        for(j = 0; j < n; j++)
            c[i*ldc+j] = 0.0;
    
    int offsize;
    offsize = rowid*m + colid*l;
    for(i = 0; i < m; i++)
        for(j = 0; j < l; j++)
            a[i*lda+j] = offsize + i + j;
    offsize = rowid*l + colid*n;
    for(i = 0; i < l; i++)
        for(j = 0; j < n; j++)
            b[i*ldb+j] = 1.0 - 2.0*((offsize + i + j)%2);

    for(i = 0; i < p; i++) {
        if(colid == k) {
            assign(m, l, work/*ta*/, lda, a, lda);
        }
        MPI_Bcast(work/*ta*/, 1, mta, k, rowcom);
        k = (k+1) % p;
        gemm(m, l, n, work/*ta*/, lda, b, ldb, c, ldc);
        if(i == p-1) continue;
        MPI_Sendrecv(b, 1, mtb, next, 1, work/*tb*/, 1, mtb, front, 1, colcom, &st);
        assign(l, n, b, ldb, work/*tb*/, ldb);
    }

    MPI_Type_free(&mta);
    MPI_Type_free(&mtb);
    return;
}

int main() {
    int iam;
	MPI_Init(0, 0);
	MPI_Comm_rank(MPI_COMM_WORLD, &iam);
    MPI_Comm comm;
    MPI_Comm_dup(MPI_COMM_WORLD, &comm);
    int m = 5, l = 4, n = 6, p = 3;
    float a[15][12], b[12][18], c[15][18], work[100];
    cannon(m, l, n, &a[0][0], 12, &b[0][0], 18, &c[0][0], 18, work, p, comm);
    printf("\nc's = %f, %f, in %d\n", c[0][0], c[0][1], iam);
    MPI_Finalize();
    return 0;
}