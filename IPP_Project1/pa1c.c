/* Tim inzitari
* Parallel Processing Assignment 1
* part c
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>



int ARRAY_SIZE = 1000;




void getLimits(int* m_p, int* n_p, int* size, int* local_size, int my_rank, int comm_sz, MPI_Comm);
void allocate_arrays( unsigned long long** local_A_p,/* int** A_p,*/ int m, int n, int size, int local_size, int my_rank, MPI_Comm comm);

void populate_Array( unsigned long long local_A[], int m,  int n, int size, int local_size, int my_rank, MPI_Comm comm);

void factorial(unsigned long long local_A[], int local_size, int size, MPI_Comm comm);
void Print_vector(char title[], unsigned long long local_A[], int size,
      int local_size, int my_rank, MPI_Comm comm);

unsigned long long doFactorial(int n)
{
	if ( n==0)
		return 1;
	else
		return (n* doFactorial(n-1));
}


int main(void)
{
	unsigned long long* local_A;
	int m, n;
	int my_rank, comm_sz;
	int size, local_size;
	MPI_Comm comm;
	double start, finish, loc_elapsed, elapsed;

  	

	MPI_Init(NULL, NULL);
	comm = MPI_COMM_WORLD;
  	MPI_Comm_size(comm, &comm_sz);
  	MPI_Comm_rank(comm, &my_rank);


  for (ARRAY_SIZE = 1000; ARRAY_SIZE <= 16000; ARRAY_SIZE*=2){
  	if (my_rank == 0) printf("\nArraySize: %d\n", ARRAY_SIZE);
	size = ARRAY_SIZE; 
	
	srandom(0);
  	/* get M and N*/
  	getLimits(&m, &n, &size, &local_size, my_rank, comm_sz, comm);

  	/* allocate arrays*/
  	allocate_arrays(&local_A,/* &A,*/ m, n, size, local_size, my_rank, comm);
  	populate_Array(local_A, m, n, size, local_size, my_rank, comm);

  	/* time stuff*/
  	MPI_Barrier(comm);
  	start = MPI_Wtime();

  	factorial(local_A, local_size, size, comm);
  	finish = MPI_Wtime();

  	loc_elapsed = finish- start;
  	MPI_Reduce(&loc_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, comm);

  	if (my_rank == 0)
  	{
      printf("Elapsed time = %e\n", elapsed);
    /* FILE *fptr2 = fopen("performance.txt", "a");
      fprintf(fptr2, "\nTime elapsed on: Array Size %d, Processor count %d is: %e", ARRAY_SIZE, comm_sz, elapsed); 
      fclose(fptr2); */
  	}

  	/*Print_vector("Factorial", local_A, size, local_size, my_rank, comm); */
  	free(local_A);
  	
  }
  	MPI_Finalize();

  	/*FILE *fptr = fopen("performance.txt", "a");
    fprintf(fptr, "\n\n\n");
    fclose(fptr); */ 

  	return(0);

} /* main*/ 





/* get M and N */
void getLimits(
	int* m_p, /* out*/
	int* n_p, /* out*/
	int* size, 
	int* local_size,
	int my_rank, /* in*/
	int comm_sz, /* in */
	MPI_Comm comm)/* in */
{
	if (my_rank == 0)
	{
		printf("Enter the lower limit (m):\n"); 
  		scanf("%d", m_p);
  		printf("Enter the upper limit (m):\n");
  		scanf("%d", n_p);
  		/**m_p = 15;
  		*n_p = 20;*/
  		while (*n_p <= *m_p)
  		{
  			printf("N must be greater than M.\nEnter the upper limit (m):\n");
  			scanf("%d", n_p);
  		}	
	}
	MPI_Bcast(m_p, 1, MPI_INT, 0, comm);
  	MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

  	/* set local array size*/
  	*local_size = *size/comm_sz;





}/* get Limits*/

void allocate_arrays(
		unsigned long long** local_A_p,
		/*int** A_p, */
		int m,
		int n,
		int size,
		int local_size,
		int my_rank,
		MPI_Comm comm
	)
{
	/*if (my_rank == 0)
	{
		*A_p = malloc(size * sizeof(int));
	}
	MPI_Bcast(A_p, size+1, MPI_INT, 0, comm); */
	*local_A_p = malloc(local_size * sizeof(unsigned long long));

} /* allocate arrays*/



void populate_Array(
	unsigned long long local_A[], /* out*/
	int m, /* in*/
	int n, /* in*/
	int size,
	int local_size,
	int my_rank,/* in*/
	MPI_Comm comm) /* in*/ 
{
	unsigned long long* A = NULL;

	if (my_rank == 0)
	{
		A = malloc(size * sizeof(unsigned long long));

		/* populate matrix*/
		for (int i = 0; i < size; i++)
		{
			A[i] = ( (unsigned long long) random() % (n - m) ) + m;
		}

		/*printf("%d, %d", A[999], A[343]); */

		MPI_Scatter(A, local_size, MPI_UNSIGNED_LONG_LONG, local_A, local_size, MPI_UNSIGNED_LONG_LONG, 0, comm);
		free(A);
	}
	else 
	{
		MPI_Scatter(A, local_size, MPI_UNSIGNED_LONG_LONG, local_A, local_size, MPI_UNSIGNED_LONG_LONG, 0, comm);
	}
}


void factorial(
	unsigned long long local_A[], 
	int local_size, 
	int size,
	MPI_Comm comm)
{
	int i;
	unsigned long long* A;

	A = malloc(size*sizeof(unsigned long long));

	MPI_Allgather(local_A, local_size, MPI_UNSIGNED_LONG_LONG, A, local_size, MPI_UNSIGNED_LONG_LONG, comm);


	for(i = 0; i < local_size; i++)
	{
		local_A[i] = doFactorial(local_A[i]);
	}

}


void Print_vector(
      char      title[]     /* in */, 
      unsigned long long    local_A[] /* in */, 
      int       size           /* in */,
      int       local_size     /* in */,
      int       my_rank     /* in */,
      MPI_Comm  comm        /* in */) {
   unsigned long long* A = NULL;
   int i;

   if (my_rank == 0) {
      A = malloc(size*sizeof(unsigned long long));
      MPI_Gather(local_A, local_size, MPI_UNSIGNED_LONG_LONG,
            A, local_size, MPI_UNSIGNED_LONG_LONG, 0, comm);
      printf("\nThe vector %s\n", title);
      for (i = 0; i < size; i++)
         printf("%lld ", A[i]);
      printf("\n");
      free(A);
   }  else {
      MPI_Gather(local_A, local_size, MPI_UNSIGNED_LONG_LONG,
            A, local_size, MPI_UNSIGNED_LONG_LONG, 0, comm);
   }
}  /* Print_vector */