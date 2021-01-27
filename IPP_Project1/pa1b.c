/* Tim Inzitari */
/* Parallel Processing */
/* Project 1 Part B*/
/* (B)   Write a parallel program to multiply a mxn matrix and nx1 vector. Your program should read the matrix and the vector from a data file (mv-data.txt) and print the results on the screen (assuming you only have 2 core processors). */



#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

const int MAX_STRING = 100;
void Get_dims(int* m, int* local_m, int* n, int* local_n,
      int my_rank, int comm_sz, MPI_Comm comm, FILE *fptr);
void Allocate_arrays(double** local_Matrix, double** local_Vector, 
      double** local_Result, int local_m, int n, int local_n, 
      MPI_Comm comm);
void Read_matrix(FILE *fptr, double local_Matrix[], int m, int local_m, 
      int n, int my_rank, MPI_Comm comm);
void Read_vector(FILE *fptr, double* local_Vector,int n,int local_n,int my_rank,MPI_Comm comm);
void Print_vector(char Title[], double local_Vector[], int n, int local_n, int my_rank, MPI_Comm comm);
void multiMatrixVector(double local_Matrix[], double local_Vector[], double local_Result[], int local_m, int n, int local_n, MPI_Comm comm, int my_rank);


int main(void)
{

    int comm_sz; /* number of processes*/
    int my_rank; /* my process rank */
    int m, local_m, n, local_n;
    double* local_Matrix;
    double* local_Vector;
    double* local_Result;
    MPI_Comm comm;

    FILE *fptr = fopen("mv-data.txt", "r");

    MPI_Init(NULL, NULL);
    comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_sz);
    MPI_Comm_rank(comm, &my_rank);


    /* Get Dimensions */
    Get_dims(&m, &local_m, &n, &local_n, my_rank, comm_sz, comm, fptr);

    /*Allocate Arrays*/
    Allocate_arrays(&local_Matrix, &local_Vector, &local_Result, local_m, n, local_n, comm);

    /* read Matrix and Vector*/
    Read_matrix(fptr, local_Matrix, m, local_m, n, my_rank, comm);
    Read_vector(fptr, local_Vector, n, local_n, my_rank, comm);

    multiMatrixVector(local_Matrix, local_Vector, local_Result, local_m, n, local_n, comm, my_rank);


    Print_vector("The Result", local_Result, m, local_m, my_rank, comm);

    free(local_Matrix);
    free(local_Vector);
    free(local_Result);

    MPI_Finalize();
    fclose(fptr);
    return 0;
}

/* get dimensions of arrays */
/* for parallel storage*/
/*m = global roys of A and components of output 
 *  local m is local M
 * n is cols of A and components of x
 * local X components
 * my rank is rank of process
 * comm_sz is number of processes
 * comm communicators of all processes calling get_dims 
 */
void Get_dims(
      int*      m      /* out */, 
      int*      local_m /* out */,
      int*      n      /* out */,
      int*      local_n  /* out */,
      int       my_rank    /* in  */,
      int       comm_sz    /* in  */,
      MPI_Comm  comm       /* in  */,
      FILE *fptr)
{
    if (my_rank == 0)
    {
        fscanf(fptr,"%d", m);
        fscanf(fptr, "%d", n);
    }
    MPI_Bcast(m, 1, MPI_INT, 0, comm);
    MPI_Bcast(n,1,MPI_INT, 0, comm);

    *local_m = *m/comm_sz;
    *local_n = *n;
} /* get dims*/


/*Allocate storage for local parts of Matrix, Vector, and Result*/
void Allocate_arrays(double** local_Matrix, double** local_Vector, 
      double** local_Result, int local_m, int n, int local_n, 
      MPI_Comm comm)
{
    *local_Matrix = malloc(local_m*n*sizeof(double));
    *local_Vector = malloc(local_n * sizeof(double));
    *local_Result = malloc(local_m*sizeof(double));

} /* allocate array*/

/* Read matrix and distribute it among processes */
void Read_matrix(
      FILE      *fptr, /*in */
      double    local_Matrix[]  /* out */, 
      int       m          /* in  */, 
      int       local_m    /* in  */, 
      int       n          /* in  */,
      int       my_rank    /* in  */,
      MPI_Comm  comm       /* in  */) 
{
    double* myMatrix = NULL;
    int i, j;

    if (my_rank== 0)
    {
        /* get full matrix before distributing*/
        myMatrix = malloc(m*n*sizeof(double));
        for (i = 0; i < m; i++)
            for(j=0; j< n; j++)
                fscanf(fptr, "%lf", &myMatrix[i*n+j]);

        /* send to processes*/
        MPI_Scatter(myMatrix, local_m * n, MPI_DOUBLE, local_Matrix, local_m *n, MPI_DOUBLE, 0, comm);
        free(myMatrix);
    }
    else 
    {
        MPI_Scatter(myMatrix, local_m*n, MPI_DOUBLE, local_Matrix, local_m*n, MPI_DOUBLE, 0, comm);
    }


} /* read matrix*/


/* Read Vector */
void Read_vector(
      FILE      *fptr     /* in  */, 
      double    local_Vector[]  /* out */, 
      int       n            /* in  */,
      int       local_n      /* in  */,
      int       my_rank      /* in  */,
      MPI_Comm  comm         /* in  */)     
{
    double* myVector = NULL;
    int i;


    if (my_rank == 0) 
    {
        myVector = malloc(n*sizeof(double));

        for (i = 0; i < n; i++)
            fscanf(fptr, "%lf", &myVector[i]);

        MPI_Scatter(myVector, local_n, MPI_DOUBLE, local_Vector, local_n, MPI_DOUBLE, 0, comm);
        free(myVector);
    }
    else 
    {
        MPI_Scatter(myVector, local_n, MPI_DOUBLE, local_Vector, local_n, MPI_DOUBLE, 0, comm); 
    }

} /* read vector*/


/* print vector*/
void Print_vector(
      char      title[]     /* in */, 
      double    local_Vector[] /* in */, 
      int       n           /* in */,
      int       local_n     /* in */,
      int       my_rank     /* in */,
      MPI_Comm  comm        /* in */) {
   double* myVector = NULL;
   int i;

   if (my_rank == 0) {
      myVector = malloc(n*sizeof(double));
      MPI_Gather(local_Vector, local_n, MPI_DOUBLE,
            myVector, local_n, MPI_DOUBLE, 0, comm);
      printf("\nThe vector %s\n", title);
      for (i = 0; i < n; i++)
         printf("%f ", myVector[i]);
      printf("\n");
      free(myVector);
   }  else {
      MPI_Gather(local_Vector, local_n, MPI_DOUBLE,
            myVector, local_n, MPI_DOUBLE, 0, comm);
   }
} 


/* the multiplicatioN*/
void multiMatrixVector(
    double  local_Matrix[],
    double  local_Vector[],
    double  local_Result[],
    int     local_m,
    int     n,
    int local_n,
    MPI_Comm comm,
    int my_rank) {


    double* myVector;
    int local_i, j;

    myVector = malloc(n*sizeof(double));

    MPI_Allgather(local_Vector, local_n, MPI_DOUBLE, myVector, local_n, MPI_DOUBLE, comm);

    for (local_i = 0; local_i < local_m; local_i++)
    {
        local_Result[local_i] = 0.0;
        for (j = 0; j < n; j++)
            local_Result[local_i] += local_Matrix[local_i*n + j]*myVector[j];
    }
    free(myVector);
}