//Encontrar o maior e menor elementos dentro de um vetor de float

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include"timer.h"

long int dim; //dimensao do vetor de entrada
int nthreads; //numero de threads 
float *vetor; //vetor de valores a serem comparados

//fluxo executado pelas threads
//dividir os vetores em blocos, cada thread executara um bloco
void * tarefa(void * arg){
    long int id = (long int) arg; //identificador da thread
    float *menorValor; //variavel local para colocar o menor valor do vetor, a posicao seguinte do ponteiro ficara com o maior valor
    long int tamBloco = dim/nthreads; //dividir em blocos para cada thread executar 
    long int ini = id * tamBloco; //onde cada thread comeca a execucao
    long int fim; //elemento final (nao processado) do bloco da thread
    
    //aloca posicoes para o menor e o maior valores
    menorValor = (float*) malloc(sizeof(float) * 2);
    if (menorValor == NULL){
        printf("ERRO-- malloc\n"); exit(1);
    }

    //determina o fim do bloco que cada thread executara
    if(id == nthreads-1)
        fim = dim; //a ultima thread executa ate o final (tratamento caso o resto da divisao seja diferente de 0)
    else
        fim = ini + tamBloco;

    *menorValor = vetor[0]; //inicia o menor valor com o primeiro elemento do vetor
    *(menorValor+1) = vetor[0]; //inicia o maior valor com o primeiro elemento do vetor

    //compara os elementos do bloco
    for(long int i=ini; i<fim; i++){
        if(vetor[i]<*menorValor) //comparacao do menor valor
            *menorValor = vetor[i]; 
        if(vetor[i]>*(menorValor+1)) //comparacao do maior valor
            *(menorValor+1) = vetor[i];
    }

    //retorna o resultado da soma local
    pthread_exit((void *) menorValor);

} 

//fluxo principal
int main(int argc, char *argv[]){
    
    float menorValorSeq, maiorValorSeq; //comparacao sequencial
    float menorValorConc, maiorValorConc; //comparacao concorrente
    //double inicio, fim; //tomada de tempo
    float *retorno; //valor de retorno das threads
    pthread_t *tid; //identificadores das threads 
    
    //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
    if(argc < 3){
        printf("Digite: %s <dimensao do vetor> <numero de threads>\n", argv[0]);
        return 1; 
    }
    dim = atoi(argv[1]); 
    nthreads = atoi(argv[2]);

    //aloca o vetor de entrada
    vetor = (float*) malloc(sizeof(float) * dim); 
    if(vetor == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }
    
    //preenche o vetor de entrada com valores aleatorios
    //GET_TIME(inicio);
    srand(time(NULL));
    for(long int i=0; i<dim; i++){
        vetor[i] = (float) rand()/100000; 
    }
    
    //inicializa os menores e maiores valores com o primeiro elemento do vetor
    menorValorSeq = vetor[0];
    maiorValorSeq = vetor[0];
    menorValorConc = vetor[0];
    maiorValorConc = vetor[0];
    //GET_TIME(fim);
    //printf("Tempo de alocacao de dados: %lf\n", fim-inicio);
        
    //comparacao sequencial dos elementos 
    //GET_TIME(inicio);
    for(long int i=0; i<dim; i++){
        if(vetor[i]<menorValorSeq)
            menorValorSeq = vetor[i];
        if(vetor[i]>maiorValorSeq)
            maiorValorSeq = vetor[i];
    }
    //GET_TIME(fim);
    //printf("Tempo de execucao sequencial: %lf\n", fim-inicio);

    
    //comparacao concorrente dos elementos
    //GET_TIME(inicio);
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads); //aloca espaco para as threads
    if(tid == NULL){
        printf("ERRO--malloc\n");
        return 2;
    }

    //criar as threads
    for(long int i=0; i<nthreads; i++){
        if(pthread_create(tid+i, NULL, tarefa, (void *) i)){
            printf("ERRO--pthread_create\n");
            return 3;
        }
    }
    
    //aguardar o termino das threads
    for(long int i=0; i<nthreads; i++){
        if(pthread_join(*(tid+i), (void**) &retorno)){ 
            printf("ERRO--pthread_create\n");
            return 3;
        }
        //compara as threads
        if(*retorno < menorValorConc) //*retorno corresponde ao menor valor (o que e retornado pela thread)
           menorValorConc = *retorno;
        if(*(retorno+1) > maiorValorConc) //*(retorno+1) corresponde ao maior valor (o que e retornado pela thread + 1)
           maiorValorConc = *(retorno+1);
        free(retorno); //libera a somaLocal da funcao executada pela thread
    } 
    //GET_TIME(fim);
    //printf("Tempo de execucao concorrente: %lf\n", fim-inicio);

    //verificar corretude
    if(maiorValorSeq == maiorValorConc && menorValorSeq == menorValorConc){
        printf("--Execucao bem sucedida\n");
        printf("Menor valor: %f\n", menorValorSeq);
        printf("Maior valor: %f\n", maiorValorSeq);
    }
    else{
        printf("--Problema na execucao\n"); 
        printf("Menor valor seq: %f\n", menorValorSeq);
        printf("Maior valor seq: %f\n", maiorValorSeq);
        printf("Menor valor conc: %f\n", menorValorConc);
        printf("Maior valor conc: %f\n", maiorValorConc);
    }

    //libera as areas de memoria alocadas
    //GET_TIME(inicio);
    free(vetor);
    free(tid);
    //GET_TIME(fim);
    //printf("Tempo de finalizacao: %lf\n", fim-inicio);

    return 0;
}
