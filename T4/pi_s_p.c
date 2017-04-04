#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    unsigned int nval;
    unsigned int in;
} Args;

/* variáveis globais acessiveis por todas as threads */
long unsigned int thread_count;

void * monte_carlo_pi_parallel(void * count_args) {
    Args * args = (Args *) count_args;
	long long unsigned int i, in = 0;
	double x, y, d;
	unsigned int seed = time(NULL);
    
	for (i = 0; i < args->nval; i++) {
		x = ((rand_r(&seed) % 1000000)/500000.0)-1;
		y = ((rand_r(&seed) % 1000000)/500000.0)-1;
		d = ((x*x) + (y*y));
		if (d <= 1) {
		    in++;
		}
	}
	
	args->in = in;
	
	return NULL;
}

int main(void) {
	unsigned int nval, i;
	double pi;
	long unsigned int duracao;
	long long unsigned int in = 0;
	Args * count_args;
	pthread_t* thread_handles;
	struct timeval start, end;

	scanf("%lu %u",&thread_count, &nval);
    thread_handles = (pthread_t*)malloc(thread_count*sizeof(pthread_t));
    count_args = (Args *)malloc(thread_count*sizeof(Args));

	srand (time(NULL));

	gettimeofday(&start, NULL);
	
	/* cria as threads */
    for(i = 0; i < thread_count; i++) {
        count_args[i].nval = nval/thread_count;
        if(pthread_create(&thread_handles[i], NULL, monte_carlo_pi_parallel, (void *) &count_args[i])) {
            printf("Erro ao criar thread\n");
            exit(1);
        };
    }
    
    for(i = 0; i < thread_count; i++) {
        pthread_join(thread_handles[i], NULL);
        in += count_args[i].in;
    }
	
	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    
	pi = 4*in/((double)nval);
	printf("%lf\n%lu\n",pi,duracao);
	
    free(thread_handles);
    pthread_exit(NULL);    
    
	return 0;
}
