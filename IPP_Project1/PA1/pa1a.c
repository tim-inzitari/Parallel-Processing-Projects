/* Tim Inzitari */
/* Parallel Processing */
/* Project 1 Part A*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(void)
{

	int comm_sz; /* number of processes*/
	int length;
	int my_rank; /* my process rank */
	char buffer[MAX_STRING];
	int dest;
	char line[MAX_STRING];
	char message[MAX_STRING];
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	

	if (my_rank == 0) /* Master process to load webpage */
	{
		FILE *fptr = fopen("hello.html", "r");
		int i = 0;
		fseek(fptr, 0, SEEK_END);
		length = ftell(fptr);
		fseek(fptr,0, SEEK_SET);

		fread(buffer, 1 , length, fptr);
		
		
		for (dest = 1; dest < comm_sz; dest++)
		{
			MPI_Send(buffer, MAX_STRING, MPI_CHAR, dest, dest, MPI_COMM_WORLD);
		}
		fclose(fptr);
	}
	else /* myrank != 0*/ 
	{
		MPI_Recv(buffer, MAX_STRING, MPI_CHAR, 0, my_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		/* Print my message*/
		sprintf(message,"Greetings from process %d of %d! Your message is: \n %s", my_rank, comm_sz, buffer);
		printf("%s", message);

		/* Recieve message */
	}

	MPI_Finalize();
	return 0;
}
/* main */