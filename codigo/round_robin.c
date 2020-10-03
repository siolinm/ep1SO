#include "round_robin.h"

/* 0,05 s*/
#define quantum 50000
#define SEGUNDO_EM_MICROSSEGUNDOS 1000000

/* Round Robin */
void round_robin() {
    int todos_terminaram = 0;
    int prox = 0;
    int tempo_dormindo = 0; /* quantos microssegundos eu passei dormindo no segundo atual */

    while (!todos_terminaram) {
        todos_terminaram = 1;
        for (int atual = 0; atual < n_processos; atual++) {

            if (t0(atual) > cur_time) {
                if(todos_terminaram){
                    atual--;
                    usleep(SEGUNDO_EM_MICROSSEGUNDOS - tempo_dormindo); /* durmo o que falta para completar 1 segundo */
                    tempo_dormindo = 0; 
                    cur_time++;
                }
                else{
                    todos_terminaram = 1;
                    atual = -1;
                }
                continue;
            }

            if (tf(atual) == -1) {
                while (cur_time == t0(prox))
                    print_chegada_processo(prox++);

                setSemaforo(atual);
                usleep(quantum); /* durmo por quantum */
                /* quantum microssegundos se passaram, se isso não completou 1 segundo somo quantum a tempo dormindo, 
                senão guardo quantos milisegundos extrapolaram 1 segundo e atualizo a quantidade de segundos atual  */
                if(SEGUNDO_EM_MICROSSEGUNDOS - (tempo_dormindo + quantum) > 0){
                    tempo_dormindo += quantum;
                }
                else{
                    tempo_dormindo += quantum;
                    tempo_dormindo -= SEGUNDO_EM_MICROSSEGUNDOS;
                    cur_time++;
                }

                ellapsed(atual) += quantum;
                if (ellapsed(atual) >= dt(atual)*SEGUNDO_EM_MICROSSEGUNDOS) {
                    tf(atual) = cur_time; /* tempo final é arredondado para baixo */
                    print_finalizacao_processo(atual);
                }
                else todos_terminaram = 0;
            }
        }
    }
}