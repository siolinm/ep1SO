#include "round_robin.h"

#define quantum 2

/* Round Robin */
void round_robin() {
    int todos_terminaram = 0;
    int prox = 0;

    while (!todos_terminaram) {
        todos_terminaram = 1;
        for (int atual = 0; atual < n_processos; atual++) {

            if (t0(atual) > cur_time) {
                if(todos_terminaram){
                    atual--;
                    dorme(); cur_time++;
                }
                else{
                    todos_terminaram = 1;
                    atual = -1;
                }
                continue;
            }

            for (int j = 0; j < quantum && tf(atual) == -1; j++) {
                while (cur_time == t0(prox))
                    print_chegada_processo(prox++);

                setSemaforo(atual);
                dorme();
                cur_time++;

                ellapsed(atual) += 1;
                if (ellapsed(atual) == dt(atual)) {
                    tf(atual) = cur_time;
                    print_finalizacao_processo(atual);
                    break;
                }
                else if(j == quantum - 1) todos_terminaram = 0;
            }
        }
    }
}