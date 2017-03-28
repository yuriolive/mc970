#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
    double min;
    double max;
    int *vet;
    int nbins;
    double h;
    double *val;
    int sval;
    int fval;
} Args;

/* variável global acessivel por todas as threads */
long unsigned int thread_count;
pthread_mutex_t * mutex;

/* funcao que calcula o minimo valor em um vetor */
double min_val(double * vet,int nval) {
	int i;
	double min;

	min = FLT_MAX;

	for(i=0;i<nval;i++) {
		if(vet[i] < min)
			min =  vet[i];
	}
	
	return min;
}

/* funcao que calcula o maximo valor em um vetor */
double max_val(double * vet, int nval) {
	int i;
	double max;

	max = FLT_MIN;

	for(i=0;i<nval;i++) {
		if(vet[i] > max)
			max =  vet[i];
	}
	
	return max;
}

/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
int * count(double min, double max, int * vet, int nbins, double h, double * val, int nval) {
	int i, j, count;
	double min_t, max_t;

	for(j=0;j<nbins;j++) {
		count = 0;
		min_t = min + j*h;
		max_t = min + (j+1)*h;
		for(i=0;i<nval;i++) {
			if(val[i] <= max_t && val[i] > min_t) {
				count++;
			}
		}

		vet[j] = count;
	}

	return vet;
}

/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
void * count_parallel(void * count_args) {
	Args * args = (Args *) count_args;
	int i, j, count, *vet, nbins, sval, fval, group;
	double min, max, h, *val, min_t, max_t;
	
	min = args->min;
    max = args->max;
    vet = args->vet;
    nbins = args->nbins;
    h = args->h;
    val = args->val;
    sval = args->sval;
    fval = args->fval;

	for(j=0;j<nbins;j++) {
		count = 0;
		min_t = min + j*h;
		max_t = min + (j+1)*h;
		for(i=sval;i<fval;i++) {
			if(val[i] <= max_t && val[i] > min_t) {
				count++;
			}
		}

		pthread_mutex_lock(&mutex[j]);
		vet[j] += count;
		pthread_mutex_unlock(&mutex[j]);
	}

}

int main(int argc, char * argv[]) {
	double h, *val, max, min;
	int n, nval, i, *vet;
	long unsigned int duracao, thread;
	struct timeval start, end;
	pthread_t∗ thread_handles;
	Args * count_args;

    /* numero de threads */
	scanf("%lu",&thread_count);
	/* entrada do numero de dados */
	scanf("%d",&nval);
	/* numero de barras do histograma a serem calculadas */
	scanf("%d",&n);

	/* vetor com os dados */
	val = (double *)malloc(nval*sizeof(double));
	vet = (int *)malloc(n*sizeof(int));
	mutex = (pthread_mutex_t *)malloc(n*sizeof(pthread_mutex_t));
	thread_handles = malloc(thread_count*sizeof(pthread_t));
	count_args = malloc(thread_count*sizeof(Args));

	/* entrada dos dados */
	for(i=0;i<nval;i++) {
		scanf("%lf",&val[i]);
	}

	for(i=0; i<n;i++) {
		pthread_mutex_init(&mutex[i], NULL);
	}

	/* calcula o minimo e o maximo valores inteiros */
	min = floor(min_val(val,nval));
	max = ceil(max_val(val,nval));

	/* calcula o tamanho de cada barra */
	h = (max - min)/n;

	/* grupo de valores a ser executado por cada threads */
	group = floor(nval/thread_count);


	gettimeofday(&start, NULL);

    /* cria as threads */
    for(thread = 0; thread < thread_count - 1; thread++) {
    	count_args[thread]->min = min;
    	count_args[thread]->max = max;
    	count_args[thread]->vet = vet;
    	count_args[thread]->nbins = n;
    	count_args[thread]->h = h;
    	count_args[thread]->val = val;
    	count_args[thread]->sval = thread*group;
    	count_args[thread]->fval = (thread+1)*group - 1;
        pthread_create(&thread_handles[thread], NULL, count_parallel, &count_args[thread]);
    }

	count_args[thread]->min = min;
	count_args[thread]->max = max;
	count_args[thread]->vet = vet;
	count_args[thread]->nbins = n;
	count_args[thread]->h = h;
	count_args[thread]->val = val;
	count_args[thread]->sval = thread*group;
	count_args[thread]->fval = (thread+1)*group - 1 + nval%thread_count;    
    pthread_create(&thread_handles[thread], NULL, count_parallel, &count_args[thread]);

    /* junta as threads */
    for(thread = 0; thread < thread_count; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }
        
    
	/* chama a funcao */
	//vet = count(min, max, vet, n, h, val, nval);

	gettimeofday(&end, NULL);

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - \
	(start.tv_sec * 1000000 + start.tv_usec));

	printf("%.2lf",min);	
	for(i=1;i<=n;i++) {
		printf(" %.2lf",min + h*i);
	}
	printf("\n");

	/* imprime o histograma calculado */	
	printf("%d",vet[0]);
	for(i=1;i<n;i++) {
		printf(" %d",vet[i]);
	}
	printf("\n");

	/* imprime o tempo de duracao do calculo */
	printf("%lu\n",duracao);

	free(vet);
	free(val);
	free(thread_handles);
	for(i=0; i<n;i++) {
		pthread_mutex_destroy(&mutex[i], NULL);
	}
	pthread_exit(NULL);

	return 0;
}
