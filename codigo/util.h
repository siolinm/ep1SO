#ifndef _UTIL_H
#define _UTIL_H

#define nmax 1000

typedef struct processo {
	char nome[31];
	int t0, tf, dt, deadline;
	int rt;                      /* remaning time */
	int ellapsed;                /* quanto tempo o precesso já executou */
	int tempo_de_entrada_na_CPU; /* salva o tempo atual no momento que o processo entrou na CPU */
	int cpu;
} Processo;

/* numero total de processos */
int n_processos;

/* numero de mudancas de contexto */
int mc;

/* modo de saida - d para impressao de informacoes na saida padrao de erro*/
char mode;

/* tipo de escalonador */
int escalonador;

/* o tempo atual da simulação */
int cur_time;

#endif
