#include "fcfs.h"

/* First Come First Served */
void fcfs() {
    int atual = 0;
    int prox = 0;

    print_chegada_processo(prox++);
    setSemaforo(atual);

    while (atual != n_processos) {
        while (cur_time == t0(prox))
            print_chegada_processo(prox++);

        dorme();
        cur_time++;

        if (t0(atual) <= cur_time) {
            ellapsed(atual) += 1;
            if (ellapsed(atual) == dt(atual)) {
                tf(atual) = cur_time;
                print_finalizacao_processo(atual);
                if(atual < n_processos - 1)
                    setSemaforo(++atual);
                else
                    atual = n_processos;
            }
        }
    }    
}
