/* Tim inzitari
* Parallel Processing Assignment 2
* part b
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>
#include "timer.h"

const int MAX_THREADS = 1024;
int ARRAY_SIZE = 1000;

long thread_count;
long double* A;
void* (*myFunction)(void*) = NULL;
long double sum;
int flag;

void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <m> <n> <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */



long double doFactorial(int n)
{
	
	if (n==1){
		
		return 1;
	}
	else
		return((long double)n * doFactorial(n-1));
}


int main(int argc, char* argv[])
{
	
	int n,m;
	
	double start, finish;
	srandom(0);

		// get Thread counts
	if (argc != 4 ) Usage(argv[0]);
   		thread_count = atoi(argv[1]);

	m = strtol(argv[1], NULL, 10);
	n = strtol(argv[2], NULL, 10);
	thread_count = strtol(argv[3], NULL, 10);



   	// assign mode using function pointer



   	/* get N and M */
   	printf("\nm=%d n = %d \n", m, n);
	

    printf("\n");

   

  	//FILE *fptr2 = fopen("pa3-performance.txt", "a");


  	//FILE *fptr = fopen("pa3.csv", "a");

	for(ARRAY_SIZE = 1000; ARRAY_SIZE <= 16000; ARRAY_SIZE*=2){
		
		A = malloc(ARRAY_SIZE * sizeof(long double));


    	// random value for Array
    	for(int i = 0; i < ARRAY_SIZE; i++)
    		A[i] = ( (unsigned long long) random() % (n - m) ) + m;
 

    	GET_TIME(start);
    	// do the thing
    	int i;
#pragma omp parallel for num_threads(thread_count) private(i)  shared(A, m, n) schedule(dynamic,1)
    	for (i = 0; i < ARRAY_SIZE; i++)
    		A[i] = doFactorial(A[i]);    	// thread create and function pass in


		GET_TIME(finish);
		printf("Array size %d, Thread Count: %ld elapsed time for sum = %e seconds\n", ARRAY_SIZE, thread_count, finish - start);

		
      	//fprintf(fptr2, "Array size %d, Thread Count: %ld, --> elapsed time for sum = %e seconds\n", ARRAY_SIZE, thread_count, finish - start); 
      	//fprintf(fptr, "%e,", finish-start);
		free(A);
		//printf("Sum: %Lf\n", sum);
		printf("Finish %d\n", ARRAY_SIZE);


		sum = 0;
		
		   // Calculation and output for the time the program takes to execute
		i = 0;
	}
	//fprintf(fptr2, "\n\n\n"); 
	//fprintf(fptr, "\n");
	//fclose(fptr);
	//fclose(fptr2); 


	return 0;
}





