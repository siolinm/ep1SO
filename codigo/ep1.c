#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include "util.h"
/* #include "fila.h" */

Processo processo[nmax];
pthread_t threads[nmax];

void load(char * nome){
    FILE * arquivo;
    int i = 0;
    arquivo = fopen(nome, "r");
    if(!arquivo) /* erro ao abrir arquivo */
        return;
    n = 0;
    while(!feof(arquivo) && fscanf(arquivo, "%s %d %d %d\n", processo[i].nome, &processo[i].t0, &processo[i].dt, &processo[i].deadline)){ 
        if(mode == 'd')      
            fprintf(stderr, "Processo %d chegou ao sistema: %s %d %d %d\n", i, processo[i].nome, processo[i].t0, processo[i].dt, processo[i].deadline);        
        i++;
        n++;
    }
}

void save(char * nome){
    FILE * arquivo;
    arquivo = fopen(nome, "w");

    for(int i = 0; i < n; i++)
        fprintf(arquivo, "%s %d %d\n", processo[i].nome, processo[i].tf, processo[i].tf - processo[i].t0);
    
    fprintf(arquivo, "%d", mc);
}

int main(int argc, char * argv[]){
    escalonador = atoi(argv[1]);
    mode = 'a';
    if(argc > 4)
        mode = argv[4][0];
    load(argv[2]);

    mc = 0;


    save(argv[3]);

    return 0;
}
