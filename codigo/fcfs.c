#include "fcfs.h"

/* First Come First Served */
void fcfs() {
    int atual = 0;
    int prox = 0;

    setSemaforo(atual);

    while (semaforo != n_processos) {
        while (cur_time == t0(prox))
            print_chegada_processo(prox++);

        dorme();

        cur_time++;
        ellapsed(atual) += 1;
        if (ellapsed(atual) == dt(atual)) {
            tf(atual) = cur_time;
            print_finalizacao_processo(atual);
            setSemaforo(++atual);
        }
    }
}