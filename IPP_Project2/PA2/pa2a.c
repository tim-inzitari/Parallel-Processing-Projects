/* Tim inzitari
* Parallel Processing Assignment 2
* part a
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include "timer.h"

const int MAX_THREADS = 1024;
int ARRAY_SIZE = 1000;

long thread_count;
long double* A;


void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */



void* factorial(void* params);
long double doFactorial(int n)
{
	
	if (n==0){
		
		return 1;
	}
	else
		return((long double)n * doFactorial(n-1));
}


int main(int argc, char* argv[])
{
	int n,m, i, j;
	long thread;
	pthread_t* thread_handles;
	double start, finish;
	srandom(0);

	/* get N and M */
	printf("Enter m and n between 1 and 1500\n");
	scanf("%d %d", &m, &n);

	// get Thread counts
	if (argc != 2) Usage(argv[0]);
   	thread_count = atoi(argv[1]);
    thread_handles = malloc(thread_count*sizeof(pthread_t));

    printf("\n");

   


  //	FILE *fptr2 = fopen("performance.txt", "a");






	for(ARRAY_SIZE = 1000; ARRAY_SIZE <= 16000; ARRAY_SIZE*=2){
		GET_TIME(start)
		A = malloc(ARRAY_SIZE * sizeof(long double));


    	// random value for Array
    	for(int i = 0; i < ARRAY_SIZE; i++)
    		A[i] = (long double)((rand() % (n-m +1)) + m);
    	//printf("\n Start %Le\n", A[423]);

    	// thread create and function pass in
		for(thread = 0; thread < thread_count; thread++){
			pthread_create(&thread_handles[thread], NULL, factorial, (void*)thread);
			
		}
		for(thread = 0; thread < thread_count; thread++){
			pthread_join(thread_handles[thread], NULL);
			
		}
		//printf(" After Factorial%Le\n", A[423]);
		GET_TIME(finish);
		printf("Array size %d, Thread Count: %ld > elapsed time = %e seconds\n", ARRAY_SIZE, thread_count, finish - start);

		
      	//fprintf(fptr2, "\nTime elapsed on: Array Size %d, thread count %ld is: %e", ARRAY_SIZE, thread_count, finish - start); 
      	
		free(A);
		printf("Finish %d\n", ARRAY_SIZE);
		
		   // Calculation and output for the time the program takes to execute

	}
	//fprintf(fptr2, "\n\n\n"); 
	//fclose(fptr2); 
	free(thread_handles);

	return 0;
}



void* factorial(void* rank)
{
   long my_rank = (long) rank;
   int i;
   int local_m = ARRAY_SIZE/thread_count;
   int my_first = my_rank * local_m ;
   int my_last = my_first + local_m -1;




   
   for (i = my_first; i < my_last; i+=1)
      A[i] = doFactorial(A[i]);

	return NULL;
}