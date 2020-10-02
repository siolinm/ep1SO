#include "util.h"

void setSemaforo(int value) {
    if(semaforo == value) return;

    if (semaforo != -1){
        // printar cpu(semaforo);
        if(mode == 'd')
            fprintf(stderr, "A thread %d parou de executar na CPU %d\n\n", semaforo, cpu(semaforo));
        if (tf(semaforo) == -1)
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

        if (mode == 'd')
            fprintf(stderr, "Processo %d chegou ao sistema: %s %d %d %d\n",
                n_processos, nome(n_processos), t0(n_processos),
                dt(n_processos), deadline(n_processos));

        n_processos++; /* Incrementando o número de processos */
    }

    fclose(arquivo);
}

void save(char * nome) {
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
    fprintf(arquivo, "\n%d\n", deadlines);

    fclose(arquivo);
}

void how_to_use() {
    fprintf(stderr, "Faltando parâmetros, para usar, digite:"
        "\n./ep1 <num-escalonador> <arquivo-entrada> <arquivo-saida> (d)"
        "\nonde d é um parâmetro opcional.\n");
}

void print_chegada_processo(int PID) {
    if (mode == 'd')
        fprintf(stderr, "Processo %s chegou no sistema no instante %d.\n"
                    "Conteudo do trace: %s %d %d %d\n\n",
                    processo[PID].nome, cur_time,
                    processo[PID].nome, processo[PID].t0, processo[PID].dt, processo[PID].deadline);
}

void print_finalizacao_processo(int PID) {
    if (mode == 'd')
        fprintf(stderr, "Processo %s finalizado no instante %d.\n"
                    "Conteudo da saída: %s %d %d\n\n",
                    processo[PID].nome, cur_time,
                    processo[PID].nome, processo[PID].tf, processo[PID].tf - processo[PID].t0);
}
