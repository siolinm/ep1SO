#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>

#include "util.h"
#include "fila_rr.h"
/* #include "fila.h" */

#define nome(i) processo[i].nome
#define dt(i) processo[i].dt
#define t0(i) processo[i].t0
#define deadline(i) processo[i].deadline
#define tf(i) processo[i].tf
#define rt(i) processo[i].rt
#define ellapsed(i) processo[i].ellapsed
#define entrada_CPU(i) processo[i].tempo_de_entrada_na_CPU
#define cpu(i) processo[i].cpu

Processo processo[nmax];
pthread_t threads[nmax];
pthread_mutex_t mutex[nmax];
int semaforo = 0; /* se semaforo == PID da thread i, enquanto ela está na CPU */
int ultimo_executando = -1;
int entrei;

void setSemaforo(int value) {
    if(semaforo == value) return;

    if (semaforo != -1){
        // printar cpu(semaforo);
        if(mode == 'd')
            fprintf(stderr, "A thread %d parou de executar na CPU %d\n\n", semaforo, cpu(semaforo));
        pthread_mutex_lock(&mutex[semaforo]);
    }

    if (value != -1){
        entrei = 1;
        pthread_mutex_unlock(&mutex[value]);
    }

    if (value != -1 && ultimo_executando != -1 && (tf(ultimo_executando) == -1 || tf(ultimo_executando) == cur_time)) {
        mc++;
        if (mode == 'd') {
            fprintf(stderr, "Mudança de contexto (total = %d).\n"
                            "Da thread %d para a %d no instante %d.\n\n",
                            mc, ultimo_executando, value, cur_time);
        }
    }

    semaforo = value;
    if (value != -1){
        entrada_CPU(value) = cur_time;
        ultimo_executando = value;
    } 
}

void load(char * nome) {
    FILE * arquivo;

    arquivo = fopen(nome, "r");
    if(arquivo == NULL) /* erro ao abrir arquivo */
        return;

    n_processos = 0;
    while(!feof(arquivo) &&
        fscanf(arquivo, "%s %d %d %d\n", nome(n_processos), &t0(n_processos),
        &dt(n_processos), &deadline(n_processos))){
        tf(n_processos) = -1;
        rt(n_processos) = dt(n_processos);
        ellapsed(n_processos) = 0;

        if(mode == 'd')
            fprintf(stderr, "Processo %d chegou ao sistema: %s %d %d %d\n",
                n_processos, nome(n_processos), t0(n_processos),
                dt(n_processos), deadline(n_processos));

        n_processos++; /* Incrementando o número de processos */
    }

    fclose(arquivo);
}

void save(char * nome){
    FILE * arquivo;
    arquivo = fopen(nome, "w");
    int deadlines = 0;


    if (arquivo == NULL)
        fprintf(stderr, "Falha ao abrir o arquivo de saída.\n");

    for(int i = 0; i < n_processos; i++){
        fprintf(arquivo, "%s %d %d\n", processo[i].nome, tf(i), tf(i) - t0(i));
        deadlines += (tf(i) > deadline(i));
    }

    fprintf(arquivo, "%d", mc);
    
    /* tirar depois */
    fprintf(arquivo, "\n%d", deadlines);

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
    // int dummy = 0;
    int PID = ((int *) argv)[0];
    int cpu;
    free(argv);

    while (tf(PID) == -1) {
        pthread_mutex_lock(&mutex[PID]);
        cpu = sched_getcpu();
        if(entrei == 1 && mode == 'd'){
            fprintf(stderr, "A thread %d começou a usar a CPU %d\n\n", PID, cpu);
            cpu(PID) = cpu;
            entrei = 0;
        }
        else if (cpu != cpu(PID) && mode == 'd'){
            fprintf(stderr, "A thread %d parou de usar a CPU %d e começou a usar a CPU %d\n\n", PID, cpu(PID), cpu);
            cpu(PID) = cpu;
        }
        pthread_mutex_unlock(&mutex[PID]);
    }

    pthread_mutex_destroy(&mutex[PID]);

    return NULL;
}

/* First Come First Served */
void fcfs() {
    int atual = 0;
    int prox = 0;

    setSemaforo(atual);

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
                if(todos_terminaram){
                    atual--;
                    sleep(1);
                    cur_time++;
                }
                else{
                    todos_terminaram = 1;
                    atual = -1;
                }
                continue;
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
                else if(j == quantum - 1) todos_terminaram = 0;
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
        pthread_mutex_init(&mutex[i], NULL);
        pthread_mutex_lock(&mutex[i]);
        pthread_create(&threads[i], NULL, busy, (void *) temp);
    }

    /*
        a simulação avança 
    */
    cur_time = t0(0);
    entrei = 1;

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
