#include <malloc.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/*
  #define NUM_THREADS 8
  #define NUM_VARS 1 //*/
#define ITERATIONS 10000000

struct Thread_arg {
	int * var;
	int id;
	long misses;
};

#define memcon __ATOMIC_SEQ_CST

void *fun(void * arg_passed){
	struct Thread_arg *arg = (struct Thread_arg *)arg_passed;
	int id = arg->id;
	int * var = arg->var;
	int zero = 0;
	long misses = 0;

	//printf("pre loop\n");
	//try to write my id
	for (int i = 0; i < ITERATIONS; i++) {
		__atomic_compare_exchange_n(var, &zero, id, sizeof(int), memcon, memcon);
		*var = 0;
		//printf("thread %d missed %ld times\n", id, misses);
	}
	//printf("post loop\n");

	arg->misses = misses;

	return NULL;
}

struct Thread_arg *args;

int main(int argc, char **argv) {
	int i;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s nr_threads nr_vars\n", argv[0]);
		return EXIT_FAILURE;
	}

	int NUM_THREADS = atoi(argv[1]), NUM_VARS = atoi(argv[2]);
	pthread_t thread_id[NUM_THREADS];
	struct Thread_arg args[NUM_THREADS];
	int *vars[NUM_VARS];

	for (i = 0; i < NUM_VARS; i++){
		vars[i] = (int *)calloc(0x1000, 1);
	}

	for (i = 0; i < NUM_THREADS; i++){
		args[i].var = vars[i % NUM_VARS];
		args[i].id = i;
	}

	struct timespec start, end, diff;
	clock_gettime(CLOCK_REALTIME, &start);

	for (i = 0; i < NUM_THREADS; i++){
		pthread_create(&thread_id[i], NULL, fun, &args[i]);
	}

	for (i = 0; i < NUM_THREADS; i++){
		pthread_join(thread_id[i], NULL);
	}

	clock_gettime(CLOCK_REALTIME, &end);

	if ((end.tv_nsec - start.tv_nsec) < 0) {
		diff.tv_sec = end.tv_sec - start.tv_sec - 1;
		diff.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	} else {
		diff.tv_sec = end.tv_sec - start.tv_sec;
		diff.tv_nsec = end.tv_nsec - start.tv_nsec;
	}

	printf("%ld.%ld\n", diff.tv_sec, diff.tv_nsec);
}
