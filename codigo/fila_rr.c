#include "fila_rr.h"

void init(){
    cabeca = malloc(sizeof(*cabeca));
    fim = cabeca;
    cabeca->prox = cabeca;
    n = 0;
}

void push(int i){
    queue_node * novo;
    novo = malloc(sizeof(queue_node));
    
    novo->prox = fim->prox;
    novo->proc = i;

    fim->prox = novo;
    fim = novo;
    
    n++;
}

int isEmpty(){
    return n == 0;
}

int pop(){
    queue_node * aux;
    if(isEmpty()) return -1;
    int x = cabeca->prox->proc;
    
    aux = cabeca->prox;
    cabeca->prox = aux->prox;
    free(aux);

    n--;
    return x;
}

int top(){
    return cabeca->prox->proc;
}

void circula(){
    if(!isEmpty()) push(pop());
}

void libera(){
    fim->prox = NULL;
    fim = cabeca;
    while(fim != NULL){
        cabeca = fim;
        fim = fim->prox;
        free(cabeca);
    }
}