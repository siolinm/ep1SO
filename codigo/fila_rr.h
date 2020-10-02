#ifndef _FILA_H
#define _FILA_H
#include "util.h"
#include <stdlib.h>

/* número de elementos na fila */
int n;

typedef struct queue_node{
    struct  queue_node * prox;
    int proc;
} queue_node;

/* ponteiro para o fim da fila e cabeça da fila */
queue_node * fim, * cabeca;

void init();

int isEmpty();

void push(int i);

int pop();

int top();

void circula();

void libera();


#endif
