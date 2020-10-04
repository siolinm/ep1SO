#ifndef _UTIL_H
#define _UTIL_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define nmax 1024
#define dorme() usleep(1000000)

typedef struct processo {
	char nome[31];
	int t0, tf, dt, deadline;    /* características do enunciado */
	int rt;                      /* remaning time */
	int ellapsed;                /* quanto tempo o processo já executou */
	int tempo_de_entrada_na_CPU; /* salva o tempo atual no momento que o processo entrou na CPU */
	int cpu;
} Processo;

Processo processo[nmax];
pthread_t threads[nmax];
pthread_mutex_t mutex[nmax];

/* Algumas macros para facilitar o uso do vetor de processos */
#define nome(i) processo[i].nome
#define dt(i) processo[i].dt
#define t0(i) processo[i].t0
#define deadline(i) processo[i].deadline
#define tf(i) processo[i].tf
#define rt(i) processo[i].rt
#define ellapsed(i) processo[i].ellapsed
#define entrada_CPU(i) processo[i].tempo_de_entrada_na_CPU
#define cpu(i) processo[i].cpu

int semaforo; /* se semaforo == PID da thread i, enquanto ela está na CPU */
int ultimo_executando;
int entrei;

/* Numero total de processos */
int n_processos;

/* Numero de mudancas de contexto */
int mc;

/* Modo de saida - d para impressao de informacoes na saida padrao de erro*/
char mode;

/* Tipo de escalonador */
int escalonador;

/* O tempo atual da simulação */
int cur_time;

/* Funções auxiliares */
void setSemaforo(int value);
void load(char *nome);
void save(char *nome);
void how_to_use();
void print_chegada_processo(int PID);
void print_finalizacao_processo(int PID);

#endif
