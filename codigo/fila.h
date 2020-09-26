#ifndef _FILA_H
#define _FILA_H
#include "util.h"

/* o vetor da fila guarda o índice do processo no vetor processos */
int fila[nmax];

/* número de elementos na fila */
int n;

/* ponteiros para o inicio e o fim da fila */
int ini, fim;

void init();

int isEmpty();

void push(int i);

int pop();


#endif