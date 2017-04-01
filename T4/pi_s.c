#include<stdio.h>
#include<stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    unsigned int sval;
    unsigned int fval;
} Args;

/* variáveis globais acessiveis por todas as threads */
long unsigned int thread_count;
long long unsigned int in = 0;
pthread_mutex_t mutex;


long long unsigned int monte_carlo_pi(unsigned int n) {
	long long unsigned int in = 0, i;
	double x, y, d;

	for (i = 0; i < n; i++) {
		x = ((rand() % 1000000)/500000.0)-1;
		y = ((rand() % 1000000)/500000.0)-1;
		d = ((x*x) + (y*y));
		if (d <= 1) in+=1;
	}

	return in;
}

void * monte_carlo_pi_parallel(void * count_args) {
    Args * args = (Args *) count_args;
	long long unsigned int i;
	double x, y, d;

	for (i = args->sval; i <= args->fval; i++) {
		x = ((rand_r(&(args->sval)) % 1000000)/500000.0)-1;
		y = ((rand_r(&(args->sval)) % 1000000)/500000.0)-1;
		d = ((x*x) + (y*y));
		if (d <= 1) {
		    pthread_mutex_lock(&mutex);
		    in = in + 1;
		    pthread_mutex_unlock(&mutex);
		}
	}
	
	return NULL;
}

int main(void) {
	unsigned int nval, i, group;
	double pi;
	long unsigned int duracao;
	Args * count_args;
	pthread_t* thread_handles;
	struct timeval start, end;

	scanf("%lu %u",&thread_count, &nval);
    thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
    count_args = (Args *)malloc(thread_count*sizeof(Args));

    
    /* inicia o mutex */
    pthread_mutex_init(&mutex, NULL);
    
    /* grupo de valores a ser executado por cada threads */
    group = floor(nval/thread_count);

	srand (time(NULL));

	gettimeofday(&start, NULL);
	
	/* cria as threads */
    for(i = 0; i < thread_count - 1; i++) {
    	count_args[i].sval = i*group;
        count_args[i].fval = (i+1)*group - 1;
        pthread_create(&thread_handles[i], NULL, monte_carlo_pi_parallel, &count_args[i]);
    }
	count_args[i].sval = i*group;
	count_args[i].fval = (i+1)*group - 1 + nval%thread_count;
    pthread_create(&thread_handles[i], NULL, monte_carlo_pi_parallel, &count_args[i]);
    
    for(i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
    }
	
	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    
	pi = 4*in/((double)nval);
	printf("%lf\n%lu\n",pi,duracao);
	
	
    free(thread_handles);
    pthread_mutex_destroy(&mutex);
    pthread_exit(NULL);    
    
	return 0;
}
