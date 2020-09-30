#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "pthread.h"
#include "util.h"
/* #include "fila.h" */

#define dt(i) processo[i].dt
#define t0(i) processo[i].t0
#define dealine(i) processo[i].deadline
#define tf(i) processo[i].tf
#define rt(i) processo[i].rt
#define ellapsed(i) processo[i].ellapsed
#define entrada_CPU(i) processo[i].tempo_de_entrada_na_CPU



Processo processo[nmax];
pthread_t threads[nmax];
pthread_mutex_t mutex[nmax];
int semaforo = 0; /* se semaforo == PID da thread i, enquanto ela está na CPU */

void setSemaforo(int value) {
    if (semaforo != -1) pthread_mutex_lock(&mutex[semaforo]);

    if (value != -1) pthread_mutex_unlock(&mutex[value]);

    if (value != semaforo && value != -1) {
        mc++;
        if (mode == 'd') {
            fprintf(stderr, "Mudança de contexto (total = %d).\n"
                            "Da thread %d para a %d no instante %d.\n\n",
                            mc, semaforo, value, cur_time);
        }
    }
    semaforo = value;
    if (value != -1) processo[value].tempo_de_entrada_na_CPU = cur_time;
}

void load(char * nome) {
    FILE * arquivo;

    arquivo = fopen(nome, "r");
    if(arquivo == NULL) /* erro ao abrir arquivo */
        return;

    n_processos = 0;
    while(!feof(arquivo) &&
        fscanf(arquivo, "%s %d %d %d\n", processo[n_processos].nome, &processo[n_processos].t0,
        &processo[n_processos].dt, &processo[n_processos].deadline)) {
        tf(n_processos) = -1;
        rt(n_processos) = dt(n_processos);
        ellapsed(n_processos) = 0;

        if(mode == 'd')
            fprintf(stderr, "Processo %d chegou ao sistema: %s %d %d %d\n",
                n_processos, processo[n_processos].nome, processo[n_processos].t0,
                processo[n_processos].dt, processo[n_processos].deadline);

        n_processos++; /* Incrementando o número de processos */
    }

    fclose(arquivo);
}

void save(char * nome){
    FILE * arquivo;
    arquivo = fopen(nome, "w");

    if (arquivo == NULL)
        fprintf(stderr, "Falha ao abrir o arquivo de saída.\n");

    for(int i = 0; i < n_processos; i++)
        fprintf(arquivo, "%s %d %d\n", processo[i].nome, processo[i].tf, processo[i].tf - processo[i].t0);

    fprintf(arquivo, "%d", mc);
    fprintf(arquivo, "\n");

    fclose(arquivo);
}

void how_to_use() {
    fprintf(stderr, "Faltando parâmetros, para usar, digite:"
        "\n./ep1 <num-escalonador> <arquivo-entrada> <arquivo-saida> (d)"
        "\nonde d é um parâmetro opcional.\n");
}

void print_chegada_processo(int PID) {
    fprintf(stderr, "Processo %s chegou no sistema no instante %d.\n"
                    "Conteudo do trace: %s %d %d %d\n\n",
                    processo[PID].nome, cur_time, 
                    processo[PID].nome, processo[PID].t0, processo[PID].dt, processo[PID].deadline);
}

void print_finalizacao_processo(int PID) {
    fprintf(stderr, "Processo %s finalizado no instante %d.\n"
                    "Conteudo da saída: %s %d %d\n\n",
                    processo[PID].nome, cur_time,
                    processo[PID].nome, processo[PID].tf, processo[PID].tf - processo[PID].t0);
}

/* Função que fica trabalhando na CPU, representando um processo
 * do escalonador.
 */
void * busy(void * argv) {
    int dummy = 0;
    int PID = ((int *) argv)[0];
    free(argv);

    while (processo[PID].tf != -1) {
        pthread_mutex_lock(&mutex[PID]);
        dummy = (dummy + rand()) * rand();
        pthread_mutex_unlock(&mutex[PID]);
    }

    return NULL;
}

/* First Come First Served */
void fcfs() {
    int atual = -1;
    int proximo_processo_a_chegar = 0;

    cur_time = t0(0);
    setSemaforo(++atual);

    while (semaforo != n_processos) {
        if (mode == 'd')
            while (cur_time == t0(proximo_processo_a_chegar)) 
                print_chegada_processo(proximo_processo_a_chegar++);

        sleep(1);

        cur_time++;
        ellapsed(atual) += 1;
        if (ellapsed(atual) == dt(atual)) {
            tf(atual) = cur_time;
            print_finalizacao_processo(atual);
            setSemaforo(++atual);
        }
       
    }
}

void insere_na_fila(int prox, int fila[nmax], int *ini, int *fim) {
    int cur = *fim - 1;
    // fprintf(stderr, "Inserindo %d fim = %d ini = %d\n", prox, *fim, *ini);

    // fprintf(stderr, "cur = %d processo[prox].rt = %d processo[fila[cur]].rt = %d\n", cur, 
    //    processo[prox].rt, processo[fila[cur]].rt);
    while (cur > ((*ini) - 1) && processo[prox].rt < processo[fila[cur]].rt) {
        fila[cur+1] = fila[cur]; 
        cur--;
    }
    // fprintf(stderr, "cur parou em %d\n", cur);
    fila[cur+1] = prox;

    if (processo[prox].rt < processo[fila[cur]].rt) { /* Preempção */
        fila[cur+1] = fila[cur];
        fila[cur] = prox;
    }

    // fila = [ 0, -1 ,  ,  ,   ,  ,   , ... ]                       

    fila[++(*fim)] = -1;
}

/* Shortest Remaining Time Next */
/* O vamos usar uma fila para ordenar os processos que serão executados.
 * Nessa fila vamos armazenar os IDs dos processos, que são números
 * inteiros relativos ao array 'processo'. Nela, temos os seguintes 
 * invariantes:
 * * fila[ini] é o primeiro processo na fila
 * * fila[ini-1] é o processo em execução atualmente 
 * * fila[fim] é a primeira posição livre da fila
 * * fila[0..ini-2] são processos que já terminaram de executar
 * * fila[fim] = -1
 * 
 * Além disso, como fila[ini-1] é quem está executando, temos o 
 * invariante:
 * * semaforo = fila[ini-1]
 */
void srtn() {
    int prox;
    int fila[nmax];
    int ini, fim;

    // fila = [ a, A , B, C, D , E,   , ... ]                       
                   
    /* O primeiro processo é um corner, pois não precisa verificar se há
     * alguém com menos remaning time do que ele (já que foi o primeiro
     * a chegar).
     */

    prox = 1;
    fila[0] = 0;
    fila[1] = -1;
    ini = fim = 1;

    while (prox < n_processos && cur_time >= processo[prox].t0) {
        /*fprintf(stderr, "Fila = [");
        for (int i = 0; i < n_processos; i++) 
            fprintf(stderr, "%d (%d), ", fila[i], ((fila[i] != -1 && fila[i] < 7) ? processo[fila[i]].rt : -1));
        fprintf(stderr, "]\n"); */

        insere_na_fila(prox, fila, &ini, &fim);
        prox++;
    }
    setSemaforo(fila[ini-1]); 

    // fila = [0, -1, ... ]

    while (ini <= n_processos) {
        if (semaforo == -1 || processo[semaforo].tf != -1) {
            /* fprintf(stderr, "Semaforo = %d ", semaforo);
            fprintf(stderr, "Mudando para %d\n", fila[ini]);

            fprintf(stderr, "Fila = [");
            for (int i = 0; i < n_processos; i++) 
                fprintf(stderr, "%d (%d), ", fila[i], ((fila[i] != -1 && fila[i] < 7) ? processo[fila[i]].rt : -1));
            fprintf(stderr, "]\n"); */

            ini++;
            // fprintf(stderr, "%d\n", ini);
        }
        
        while (prox < n_processos && cur_time >= processo[prox].t0) {
            /* fprintf(stderr, "Fila = [");
            for (int i = 0; i < n_processos; i++) 
                fprintf(stderr, "%d (%d), ", fila[i], ((fila[i] != -1 && fila[i] < 7) ? processo[fila[i]].rt : -1));
            fprintf(stderr, "]\n"); */

            insere_na_fila(prox, fila, &ini, &fim);
            prox++;
        }

        // fprintf(stderr, "fila[ini-1] = %d\n", fila[ini-1]);
        setSemaforo(fila[ini-1]);
        usleep(500000);
        cur_time = time(NULL) - begin_time;
    }
}

/* Round Robin */
void round_robin() {
    int todos_terminaram = 0;
    int proximo_processo_a_chegar = 0;
    int quantum = 2;

    while (!todos_terminaram) {
        todos_terminaram = 1;
        for (int i = 0; i < n_processos; i++) {
            cur_time = time(NULL) - begin_time;

            if (processo[i].t0 > cur_time) {
                i = -1; continue;
            }

            for (int j = 0; j < quantum && processo[i].tf == -1 ; j++) {
                setSemaforo(i);
                sleep(1);
                if (processo[i].tf == -1) todos_terminaram = 0;

                cur_time = time(NULL) - begin_time;
                if (mode == 'd')
                    while (cur_time == processo[proximo_processo_a_chegar].t0) 
                        print_chegada_processo(proximo_processo_a_chegar++);
            }
        }
    }
}

int main(int argc, char * argv[]) {
    /* Checando se os argumentos foram passados corretamente */
    if (argc < 4) {
        how_to_use();
        return 0;
    }

    /* Lendo os argumentos: */
    escalonador = atoi(argv[1]); /* O número do escalonador */

    load(argv[2]);

    mode = 'a'; /* Inicialmente supomos que não é o modo de display */
    if(argc > 4) mode = argv[4][0];

    mc = 0; /* Inicializar mudanças de contexto com zero */

    semaforo = -1; /* Nenhum processo sendo executado */

    int * temp;
    for (int i = 0; i < n_processos; i++) {
        temp = malloc(sizeof(int));
        *temp = i;

        // fprintf(stderr, "Criando thread %d com dt = %d.\n", temp[0], temp[1]);
        pthread_create(&threads[i], NULL, busy, (void *) temp);
    }

    begin_time = time(NULL); /* O começo da simulação */ 
    while (time(NULL) == begin_time) usleep(5000);
    begin_time = time(NULL);

    cur_time = processo[0].t0;

    /* Escalonador entra aqui */
    switch (escalonador) {
        case 1:
            fcfs();
            break;
        case 2:
            srtn();
            break;
        case 3:
            round_robin();
            break;
        default:
            fprintf(stderr, "Argumento do escalonador errado (deve ser um número entre 1 e 3).\n");
            return 0;
    }

    for (int i = 0; i < n_processos; i++)
        pthread_join(threads[i], NULL);

    save(argv[3]); /* Salvar a saída no final */

    return 0;
}
