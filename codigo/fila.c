#include "fila.h"

void init(){
    ini = fim = 0;
}

void push(int i){
    if(ini == ((fim + 1) % nmax)) return;

    fila[fim++] = i;
    fim %= nmax;
}

int isEmpty(){
    return ini == fim;
}

int pop(){
    int x = fila[ini++];
    ini %= nmax;
    return x;
}