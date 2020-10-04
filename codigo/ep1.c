#define _GNU_SOURCE

#include <sched.h>
#include <stdio.h>

#include "fcfs.h"
#include "srtn.h"
#include "round_robin.h"


/** Função que fica trabalhando na CPU, representando um processo do escalonador.
 */
void * busy(void * argv) {
    int PID = ((int *) argv)[0];
    int cpu;
    free(argv);

    while (tf(PID) == -1) {
        pthread_mutex_lock(&mutex[PID]);
        cpu = sched_getcpu();
        if (entrei == 1 && mode == 'd') {
            fprintf(stderr, "A thread %d começou a usar a CPU %d.\n\n", PID, cpu);
            cpu(PID) = cpu;
            entrei = 0;
        }
        else if (cpu != cpu(PID) && mode == 'd') {
            fprintf(stderr, "A thread %d parou de usar a CPU %d e começou a usar a CPU %d.\n\n",
                    PID, cpu(PID), cpu);
            cpu(PID) = cpu;
        }
        pthread_mutex_unlock(&mutex[PID]);
    }

    pthread_mutex_destroy(&mutex[PID]);

    return NULL;
}

int main(int argc, char * argv[]) {
    /* Checando se os argumentos foram passados corretamente */
    if (argc < 4) {
        how_to_use();
        return 0;
    }

    /*** Lendo os argumentos: ***/
    escalonador = atoi(argv[1]); /* O número do escalonador */

    load(argv[2]);

    mode = 'a'; /* Inicialmente supomos que não é o modo de display */
    if (argc > 4) mode = argv[4][0];

    mc = 0; /* Inicializar mudanças de contexto com zero */

    semaforo = -1; /* Nenhum processo sendo executado */
    ultimo_executando = -1;

    /* Criando todas as threads */
    int * temp;
    for (int i = 0; i < n_processos; i++) {
        temp = malloc(sizeof(int));
        *temp = i;
        pthread_mutex_init(&mutex[i], NULL);
        pthread_mutex_lock(&mutex[i]);
        pthread_create(&threads[i], NULL, busy, (void *) temp);
    }

    /* A simulação avança */
    cur_time = t0(0);
    entrei = 1;

    /* Escalonador começa aqui */
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

    setSemaforo(-1); /* No final da simulação, não há nenhuma thread executando */

    /* Garantir que todas as threads finalizaram */
    for (int i = 0; i < n_processos; i++)
        pthread_join(threads[i], NULL);

    save(argv[3]); /* Salvar a saída no final */

    return 0;
}
