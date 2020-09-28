#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "pthread.h"
#include "util.h"
/* #include "fila.h" */

Processo processo[nmax];
pthread_t threads[nmax];
// pthread_mutex_t mutex[nmax];
int semaforo = 0; // se semaforo == PID da thread i, enquanto ela está na CPU

void load(char * nome) {
    FILE * arquivo;

    arquivo = fopen(nome, "r");
    if(arquivo == NULL) /* erro ao abrir arquivo */
        return;

    n_processos = 0;
    while(!feof(arquivo) &&
        fscanf(arquivo, "%s %d %d %d\n", processo[n_processos].nome, &processo[n_processos].t0,
        &processo[n_processos].dt, &processo[n_processos].deadline)) {
        processo[n_processos].tf = -1;

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

/* Função que fica trabalhando na CPU, representando um processo
 * do escalonador.
 */
void * busy(void * argv) {
    time_t begin, now;
    time_t beginaux;
    int ellapsed = 0;
    int *temp = (int *) argv;
    int PID = temp[0];
    int deltat = temp[1];

    begin = beginaux = time(NULL);

    fprintf(stderr, "Eu sou %d.\n", PID);
    while (1) {
        if (semaforo == PID) {
            now = time(NULL);

            ellapsed += now - begin;
            begin = now;

            fprintf(stderr, "Thread %d executada.\n", PID);
            if (ellapsed >= deltat) {
                fprintf(stderr, "Ellapsed = %d Dt = %d\n", ellapsed, deltat);                    
                processo[PID].tf = now - beginaux;
                semaforo = -1;
                break;
            }

            /* Algumas operações para usarem a CPU */
            (rand() + rand()) * rand();
        }
        else {
            usleep(500000);
            begin = time(NULL);
            // fprintf(stderr, "zzz semaforo = %d , sou %d\n", semaforo, PID);
        }
    }

    return NULL;
}

/* First Come First Served */
void fcfs() {
    int processo_atual = 0;
    while (semaforo != n_processos) {
        if (semaforo == -1 && cur_time >= processo[processo_atual].t0) 
            semaforo = processo_atual++;
        else 
            sleep(1);
        cur_time = time(NULL) - begin_time;
    } 
}

/* Shortest Remaining Time Next */
void srtn() {

}

/* Round Robin */
void round_robin() {
    int todos_terminaram = 0;
    int quantum = 2;

    while (!todos_terminaram) {
        todos_terminaram = 1;
        for (int i = 0; i < n_processos; i++) {
            cur_time = time(NULL) - begin_time;
            if (processo[i].t0 > cur_time) {                    
                i = -1; continue;
            }

            for (int j = 0; j < quantum && processo[i].tf == -1 ; j++) {
                semaforo = i;
                sleep(1);
                if (processo[i].tf == -1) todos_terminaram = 0;
            }
        }
    }
}

int main(int argc, char * argv[]) {
    fprintf(stderr, "argc = %d\n", argc);
    for (int i = 0; i < argc; i++) {
        fprintf(stderr, "argv[%d] = %s\n", i, argv[i]);
    }
    

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
        temp = malloc(sizeof(int) * 2);
        temp[0] = i;
        temp[1] = processo[i].dt;

        fprintf(stderr, "Criando thread %d com dt = %d.\n", temp[0], temp[1]);
        pthread_create(&threads[i], NULL, busy, (void *) temp);
    }

    begin_time = time(NULL); /* O começo da simulação */
    cur_time = processo[0].t0;
    
    /* Escalonador entra aqui */
    switch (escalonador)
    {
    case 1:
        fcfs();
        break;
    case 2:

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
