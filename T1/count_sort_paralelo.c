#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

/*
 * Nome: Yuri Soares Olive
 * RA: 148265
 * T1 - Count Sort em Paralelo
 */

/* count sort parallel */
double count_sort_parallel(double a[], int n, int nt) {
	int i, j, count;
	double *temp;
	double start, end, duracao;

	temp = (double *)malloc(n*sizeof(double));

	start = omp_get_wtime();
#   pragma omp parallel for num_threads(nt) default(none) private(i, j, count) shared(n, a, temp)
	for (i = 0; i < n; i++) {
		count = 0;
		for (j = 0; j < n; j++)
			if (a[j] < a[i])
				count++;
			else if (a[j] == a[i] && j < i)
				count++;
		temp[count] = a[i];
	}
	end = omp_get_wtime();

	duracao = end - start;

	memcpy(a, temp, n*sizeof(double));
	free(temp);

	return duracao;
}

int main(int argc, char * argv[]) {
	int i, n, nt;
	double  * a, t_p;

	scanf("%d",&nt);
	
	/* numero de valores */
	scanf("%d",&n);

	/* aloca os vetores de valores para o teste em paralelo(a) */
	a = (double *)malloc(n*sizeof(double));

	/* entrada dos valores */
	for(i=0;i<n;i++)
		scanf("%lf",&a[i]);
	
	/* chama a funcao de count sort em paralelo */
	t_p = count_sort_parallel(a,n,nt);

	/* Imprime o vetor ordenado */
	for(i=0;i<n;i++)
		printf("%.2lf ",a[i]);

	printf("\n");

	/* imprime os tempo obtido */
	printf("%lf\n",t_p);

	return 0;
}
