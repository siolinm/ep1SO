#include "round_robin.h"

/* 0,05 s*/
#define quantum 1000000
#define SEGUNDO_EM_MICROSSEGUNDOS 1000000
#define min(a, b) (a >= b ? b : a)

/* Round Robin */
void round_robin() {
    int todos_terminaram = 0;
    int prox = 0;
    int tempo_dormindo = 0; /* quantos microssegundos eu passei dormindo no segundo atual */
    int minimo;

    while (!todos_terminaram) {
        todos_terminaram = 1;
        for (int atual = 0; atual < n_processos; atual++) {

            if (t0(atual) > cur_time) {
                if(todos_terminaram){
                    atual--;
                    dorme();
                    /*usleep(SEGUNDO_EM_MICROSSEGUNDOS - tempo_dormindo);  durmo o que falta para completar 1 segundo */
                    tempo_dormindo = 0;
                    cur_time++;
                }
                else{
                    todos_terminaram = 1;
                    atual = -1;
                }
                continue;
            }

            while (cur_time == t0(prox))
                print_chegada_processo(prox++);

            if (tf(atual) == -1) {
                setSemaforo(atual);
                rt(atual) = dt(atual)*SEGUNDO_EM_MICROSSEGUNDOS - ellapsed(atual);
                minimo = min(quantum, rt(atual));
                dorme();
                /*usleep(minimo);  durmo por quantum */
                /* quantum microssegundos se passaram, se isso não completou 1 segundo somo quantum a tempo dormindo,
                senão guardo quantos milisegundos extrapolaram 1 segundo e atualizo a quantidade de segundos atual  */
                if (SEGUNDO_EM_MICROSSEGUNDOS - (tempo_dormindo + minimo) > 0) {
                    tempo_dormindo += minimo;
                }
                else {
                    tempo_dormindo += minimo;
                    tempo_dormindo -= SEGUNDO_EM_MICROSSEGUNDOS;
                    cur_time++;
                }

                ellapsed(atual) += minimo;
                if (ellapsed(atual) >= dt(atual)*SEGUNDO_EM_MICROSSEGUNDOS) {
                    tf(atual) = cur_time; /* tempo final é arredondado para baixo */
                    /*pthread_join(threads[atual], NULL);*/
                    print_finalizacao_processo(atual);
                }
                else todos_terminaram = 0;
            }
        }
    }
}
