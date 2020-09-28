#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "pthread.h"
#include "util.h"
/* #include "fila.h" */

Processo processo[nmax];
pthread_t threads[nmax];

void load(char * nome) {
    FILE * arquivo;

    arquivo = fopen(nome, "r");
    if(arquivo == NULL) /* erro ao abrir arquivo */
        return;

    n = 0;
    while(!feof(arquivo) &&
        fscanf(arquivo, "%s %d %d %d\n", processo[n].nome, &processo[n].t0,
        &processo[n].dt, &processo[n].deadline)) {

        if(mode == 'd')
            fprintf(stderr, "Processo %d chegou ao sistema: %s %d %d %d\n",
                n, processo[n].nome, processo[n].t0,
                processo[n].dt, processo[n].deadline);

        n++; /* Incrementando o número de processos */
    }

    fclose(arquivo);
}

void save(char * nome){
    FILE * arquivo;
    arquivo = fopen(nome, "w");

    if (arquivo == NULL)
        fprintf(stderr, "Falha ao abrir o arquivo de saída.\n");

    for(int i = 0; i < n; i++)
        fprintf(arquivo, "%s %d %d\n", processo[i].nome, processo[i].tf, processo[i].tf - processo[i].t0);

    fprintf(arquivo, "%d", mc);
    fprintf(arquivo, "\n");

    fclose(arquivo);
}

void how_to_use() {
    printf("Faltando parâmetros, para usar, digite:"
        "\n./ep1 <num-escalonador> <arquivo-entrada> <arquivo-saida> (d)"
        "\nonde d é um parâmetro opcional.\n");
}

/* Função que fica trabalhando na CPU, representando um processo
 * do escalonador.
 */
void busy(int dt) {
    time_t begin, now;
    int ellapsed;

    begin = time(NULL);

    while (1) {
      now = time(NULL);

      ellapsed = now-begin;

      printf("Ellapsed = %d Dt = %d\n", ellapsed, dt);
      if (ellapsed >= dt) break;

      /* Algumas operações para usarem a CPU */
      (rand() + rand()) * rand();
    }
}

int main(int argc, char * argv[]){
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

    busy(5);

    save(argv[3]); /* Salvar a saída no final */

    return 0;
}
