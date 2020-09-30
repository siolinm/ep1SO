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
int primeiro_semaforo = 1;

void setSemaforo(int value) {
    if (semaforo != -1){
        
        pthread_mutex_lock(&mutex[semaforo]);
    } 

    if (value != -1){
        pthread_mutex_unlock(&mutex[value]);
    } 

    if (value != semaforo && value != -1 && !primeiro_semaforo) {
        mc++;
        if (mode == 'd') {
            fprintf(stderr, "Mudança de contexto (total = %d).\n"
                            "Da thread %d para a %d no instante %d.\n\n",
                            mc, semaforo, value, cur_time);
        }
    }
    else if(primeiro_semaforo) primeiro_semaforo = 0;

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

    while (tf(PID) != -1) {
        pthread_mutex_lock(&mutex[PID]);
        dummy = (dummy + rand()) * rand();
        pthread_mutex_unlock(&mutex[PID]);
    }

    return NULL;
}

/* First Come First Served */
void fcfs() {
    int atual = -1;
    int prox = 0;

    setSemaforo(++atual);

    while (semaforo != n_processos) {
        if (mode == 'd')
            while (cur_time == t0(prox)) 
                print_chegada_processo(prox++);

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
    
    while (cur > ((*ini) - 1) && rt(prox) < rt(fila[cur])) {
        fila[cur+1] = fila[cur]; 
        cur--;
    }

    fila[cur+1] = prox;

    if (rt(prox) < rt(fila[cur])) { /* Preempção */
        fila[cur+1] = fila[cur];
        fila[cur] = prox;
    }

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
    int prox, atual;
    int fila[nmax];
    int ini, fim;
                   
    /* O primeiro processo é um corner, pois não precisa verificar se há
     * alguém com menos remaning time do que ele (já que foi o primeiro
     * a chegar).
     */
    prox = 1;
    fila[0] = 0;
    fila[1] = -1;
    ini = fim = 1;

    if (mode == 'd')
        print_chegada_processo(0);

    while (ini <= n_processos) {
        while (prox < n_processos && cur_time >= t0(prox)) {
            if (mode == 'd')
                print_chegada_processo(prox);
            
            insere_na_fila(prox, fila, &ini, &fim);
            prox++;
        }
        /* atualiza quem deve rodar (se houve alguma alteração) */
        atual = fila[ini-1];
        setSemaforo(atual);
        
        sleep(1);
        
        /* 1 segundo se passou */
        cur_time++;
        ellapsed(atual) += 1;
        rt(atual) -= 1;
        
        if (ellapsed(atual) == dt(atual)) {
            tf(atual) = cur_time;
            print_finalizacao_processo(atual);
            /* ini anda e agora ini-1 é o novo primeiro da fila (possivelmente fila[ini-1] é -1) */            
            atual = fila[ini++];
            setSemaforo(atual);
        }
    }
}

/* Round Robin */
void round_robin() {
    int todos_terminaram = 0;
    int prox = 0;
    int quantum = 2;

    while (!todos_terminaram) {
        todos_terminaram = 1;
        for (int atual = 0; atual < n_processos; atual++) {

            if (t0(atual) > cur_time) {
                atual = -1; continue;
            }

            for (int j = 0; j < quantum && tf(atual) == -1; j++) {
                if (mode == 'd')
                    while (cur_time == t0(prox)) 
                        print_chegada_processo(prox++);

                setSemaforo(atual);
                sleep(1);
                cur_time++;            
                
                ellapsed(atual) += 1;
                if (ellapsed(atual) == dt(atual)) {
                    tf(atual) = cur_time;
                    print_finalizacao_processo(atual);
                    break;
                }
                else todos_terminaram = 0;                
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

        pthread_create(&threads[i], NULL, busy, (void *) temp);
    }

    /*
        a simulação avança 
    */
    cur_time = t0(0);

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
