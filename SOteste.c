#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#define TAM 20


void flush_in (){
	int ch;
	while((ch = fgetc(stdin)) != EOF && ch != '\n'){}
}


//struct para os dados dos threads
typedef struct argumentos{
	int *epi;  //endereço da posiçao inicial do vetor
	int ini;
	int fim;
}thread;

struct argumentos *argthread;

int *v;


void criaHeap (int *epi, int i, int f){
	int aux = *(epi+i);
	int j = i*2 + 1;
	while(j <= f){
		if(j < f){
			if(*(epi+j) < *(epi+j+1))
				j += 1;
		}
		if(aux < *(epi+j)){
			*(epi+i) = *(epi+j);
			i = j;
			j = 2*i + 1;
		}
		else { j = f + 1; }
	}
	*(epi+i) = aux;
}


void heapsort (int *epi, int ini, int fim){ //funçao para a ordenaçao dos valores
	int i, aux;

	for(i = (fim-1)/2; i >= ini-1; i--){
		criaHeap(epi, i, fim-1);
	}
	for(i = fim-1; i >= ini; i--){
		aux = *(epi);
		*(epi) = *(epi+i);
		*(epi+i) = aux;
		criaHeap(epi, ini-1, i-1);
	}
} 


void *tfunc (void *t_arg){
	struct argumentos *t;
	t = (thread *) t_arg;

	int *epi, ini, fim;

	epi = t->epi;
	ini = t->ini;
	fim = t->fim;

	printf("valor antes do sort: epi = %d, ini = %d , fim = %d\n", *epi, ini, fim);

	heapsort(epi, ini, fim); //a partir desse ponto, o codigo nao funciona mais

	printf("valor depois do sort: epi = %d, ini = %d, fim = %d\n", *epi, ini, fim); //os valores para os epi's estao totalmente errados

	free(t_arg);
	pthread_exit(NULL);

}


int main (){
	int N, i, p, T, fim, ini;

	printf("Quantidade de valores a serem ordenados : ");
	scanf("%d", &N);

	printf("Quantidade de threads : ");
	scanf("%d", &T);

	flush_in(); //limpar buffer
        /*
         * [Andre] Na próxima vez, em sistemas Linux tente usar a função a seguir:
         *
         * __fpurge(stdin);
         */

	printf("Nome do arquivo de entrada : ");
	char ne[TAM];
	fgets(ne, TAM-1, stdin);
        
        /*[Andre] Adicionei a linha a seguir para eliminar o '\n' que ficava na entrada.*/
        ne[strlen(ne)-1]='\0';

	printf("Nome do arquivo de saída : ");
	char ns[TAM];
	fgets(ns, TAM-1, stdin);
        
        /*[Andre] Adicionei a linha a seguir para eliminar o '\n' que ficava na entrada.*/
        ns[strlen(ns)-1]='\0';

	v = (int *)malloc(N * sizeof(int));	

	//abre o arquivo de entrada, le os valores e salva em um vetor
	FILE *entrada = fopen(ne, "r");
	if(entrada == NULL)
		printf("Erro ao abrir o arquivo\n");


	for(p = 0; p < N; p++){
		fscanf(entrada, "%d", &i);
		v[p] = i;
	}
	fclose(entrada);


	//declaraçao de T threads
	pthread_t meusthreads[T];


	//indica qual a thread sendo realizada. Se cont = 1, entao eh a primeira thread, e assim por diante
	int cont = 0;


	for(p = 0; p < T; p++){
		cont++;

		argthread = (thread *)malloc(sizeof(thread));

		fim = cont * (N/T) - 1; //calculo para posiçao final ate onde a thread ira ordenas o vetor
		
		ini = fim - (N/T) + 1; //calculo para a posicao inicial a partir da qual a thread ira ordenar o vetor

		if((cont == T) && ((N - 1) - cont * ((N/T) - 1)) != 0){ //se ainda houverem valores nao considerados p/posiçao final
			fim = N - 1;
		}

		printf("cont = %d , ini = %d , fim = %d\n", cont, ini, fim);

		argthread->epi = v+ini;
		argthread->ini = ini+1;
		argthread->fim = fim+1;

		printf("Criando thread %d\n", cont);
		pthread_create(&meusthreads[p], NULL, tfunc, (void *)argthread);
	}


	for(p = 0; p < T; p++){
		pthread_join(meusthreads[p], NULL);
	}


	//fazer um merge para ordenar as partes ja ordenadas entre si


	//arquivo de saida
	FILE *saida = fopen(ns, "w");
	for(p = 0; p < N; p++){
		fprintf(saida, "%d ", v[p]);
	}
	fclose(saida);


return 0;
}
