#include "srtn.h"

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
    fila[1] = -1; ini = fim = 1;

    print_chegada_processo(0);
    while (prox < n_processos && cur_time >= t0(prox)) {
        print_chegada_processo(prox);

        insere_na_fila(prox, fila, &ini, &fim);
        prox++;
    }

    while (ini <= n_processos) {

        /* atualiza quem deve rodar (se houve alguma alteração) */
        atual = fila[ini-1];
        setSemaforo(atual);

        dorme();

        /* 1 segundo se passou */
        cur_time++;
        if (atual != -1) {
            ellapsed(atual) += 1;
            rt(atual) -= 1;
        }

        while (prox < n_processos && cur_time >= t0(prox)) {
            print_chegada_processo(prox);

            insere_na_fila(prox, fila, &ini, &fim);
            prox++;
        }

        if (atual != -1 && ellapsed(atual) == dt(atual)) {
            tf(atual) = cur_time;
            print_finalizacao_processo(atual);
            /* ini anda e agora ini-1 é o novo primeiro da fila (possivelmente fila[ini-1] é -1) */
            atual = fila[ini++];
            setSemaforo(atual);
        }
    }
}
