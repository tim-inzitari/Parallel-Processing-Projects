/* Tim inzitari
* Parallel Processing Assignment 2
* part b
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include "timer.h"
#include <semaphore.h>  /* Semaphores are not part of Pthreads */

const int MAX_THREADS = 1024;
int ARRAY_SIZE = 1000;

long thread_count;
long double* A;
void* (*myFunction)(void*) = NULL;
long double sum;
int flag;
pthread_mutex_t mutex;
sem_t semaphore;
pthread_rwlock_t rwlock;
void *noLimit(void* rank);
void *busy(void* rank);
void *mutex_function(void* rank);
void *semaphore_function(void* rank);
void *writeLock_function(void* rank);

void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <mode number 0-4>\n", prog_name);
   fprintf(stderr, "Modes are\n 0: noLimit\n 1: Busy-waiting\n 2: Mutex\n 3: Semaphore\n 4: WriteLock\n");
   exit(0);
}  /* Usage */



void* factorial(void* params);
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
	
	int n,m, i, j;
	long thread;
	int mode = -1;
	char modestring[1000];
	pthread_t* thread_handles;
	double start, finish;
	srandom(0);

	

	// get Thread counts
	if (argc != 3 ) Usage(argv[0]);
   		thread_count = atoi(argv[1]);
   		mode = atoi(argv[2]);
   	if(!(mode >= 0 && mode < 5))
   		Usage(argv[0]);
   	
   	// assign mode using function pointer
   	switch(mode)
   	{
   		case 0:
   			myFunction = noLimit;
   			strcpy(modestring,"Nothing");
   			break;
   		case 1:
   			myFunction = busy;
   			strcpy(modestring,"Busy Waiting");
   			break;
   		case 2: 
   			myFunction = mutex_function;
   			strcpy(modestring,"Mutex");
   			break;

   		case 3: 
   			myFunction = semaphore_function;
   			strcpy(modestring,"Semaphore");
   			break;

   		case 4: 
   			myFunction = writeLock_function;
   			strcpy(modestring,"WriteLock");
   			break;

   		default:
   			fprintf(stderr, "Fatal error occured with mode select");
   			exit(0);
   	}



   	/* get N and M */
   	printf("\nm=n=1\n");
	m = 1;
	n = 1;
    thread_handles = malloc(thread_count*sizeof(pthread_t));

    printf("\n");

   

  	//FILE *fptr2 = fopen("performance2.txt", "a");





	for(ARRAY_SIZE = 1000; ARRAY_SIZE <= 16000; ARRAY_SIZE*=2){
		
		A = malloc(ARRAY_SIZE * sizeof(long double));


    	// random value for Array
    	for(int i = 0; i < ARRAY_SIZE; i++)
    		A[i] = (long double) 1;
 

    	// thread create and function pass in
		for(thread = 0; thread < thread_count; thread++){
			pthread_create(&thread_handles[thread], NULL, factorial, (void*)thread);
			
		}
		for(thread = 0; thread < thread_count; thread++){
			pthread_join(thread_handles[thread], NULL);
		}


		GET_TIME(start);
		if (mode == 3)
			sem_init(&semaphore, 0, 0);
		if (mode ==4)
			pthread_rwlock_init(&rwlock, NULL);
		// get global sum
		for (thread = 0; thread < thread_count; thread++)
			pthread_create(&thread_handles[thread], NULL, myFunction, (void*)thread);

		for (thread = 0; thread < thread_count; thread++)
			pthread_join(thread_handles[thread], NULL);

		//printf(" After Factorial%Le\n", A[423]);
		if (mode == 3)
			sem_destroy(&semaphore);
		if (mode == 4)
			pthread_rwlock_destroy(&rwlock);
		GET_TIME(finish);
		printf("Array size %d, Thread Count: %ld, Mode: %s --> elapsed time for sum = %e seconds\n", ARRAY_SIZE, thread_count, modestring, finish - start);

		
      	//fprintf(fptr2, "Array size %d, Thread Count: %ld, Mode: %s --> elapsed time for sum = %e seconds\n", ARRAY_SIZE, thread_count, modestring, finish - start); 
      	
		free(A);
		//printf("Sum: %Lf\n", sum);
		printf("Finish %d\n", ARRAY_SIZE);


		sum = 0;
		
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





void *noLimit(void *rank){

	long my_rank = (long) rank;
	long double local_sum = 0;
	long long i;
	long long my_n = ARRAY_SIZE/thread_count;
	long long my_first_i = my_n *my_rank;
	long long my_last_i = my_first_i + my_n;

	for (i = my_first_i; i < my_last_i; i++){
		local_sum += A[i];
	}

	// no critical section protection
	sum += local_sum;

	return NULL;

}



void *busy(void *rank){
	long my_rank = (long) rank;
	long double local_sum = 0;
	long long i;
	long long my_n = ARRAY_SIZE/thread_count;
	long long my_first_i = my_n *my_rank;
	long long my_last_i = my_first_i + my_n;

	for (i = my_first_i; i < my_last_i; i++){
		local_sum += A[i];
	}


	// busywaiting  critical section protection
	while (flag != my_rank);
	sum += local_sum;
	flag = (flag+1)%thread_count;

	return NULL;
}



void *mutex_function(void *rank){
	long my_rank = (long) rank;
	long double local_sum = 0;
	long long i;
	long long my_n = ARRAY_SIZE/thread_count;
	long long my_first_i = my_n *my_rank;
	long long my_last_i = my_first_i + my_n;

	for (i = my_first_i; i < my_last_i; i++){
		local_sum += A[i];
	}

	// mutex critical section protection
	pthread_mutex_lock(&mutex);
	sum += local_sum;
	pthread_mutex_unlock(&mutex);

	return NULL;
}

void *semaphore_function(void *rank){

	long my_rank = (long) rank;
	long double local_sum = 0;
	long long i;
	long long my_n = ARRAY_SIZE/thread_count;
	long long my_first_i = my_n *my_rank;
	long long my_last_i = my_first_i + my_n;


 	

	for (i = my_first_i; i < my_last_i; i++){
		local_sum += A[i];
	}


	// semaphore critical section protection
	sem_post(&semaphore);
	sum += local_sum;
	sem_wait(&semaphore);


	return NULL;

}



void *writeLock_function(void *rank){
	long my_rank = (long) rank;
	long double local_sum = 0;
	long long i;
	long long my_n = ARRAY_SIZE/thread_count;
	long long my_first_i = my_n *my_rank;
	long long my_last_i = my_first_i + my_n;


 	

	for (i = my_first_i; i < my_last_i; i++){
		local_sum += A[i];
	}

	// write lock critical section protection
	pthread_rwlock_wrlock(&rwlock);
	sum += local_sum;
	pthread_rwlock_unlock(&rwlock);

	return NULL;
}