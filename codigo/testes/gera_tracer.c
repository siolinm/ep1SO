#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[]) {
    int quantidade = atoi(argv[1]);
    const char *nome_arquivo = argv[2];
    srand(time(NULL));

    FILE *saida;
    saida = fopen(nome_arquivo, "w");

    int t0, dt, deadline;
    int ultimo_t0 = (rand() % 10) + 1;
    int sum_dt = 0;

    for (int i = 0; i < quantidade; i++) {
        t0 = ultimo_t0 + (rand() % 6)*(rand() % 4);
        ultimo_t0 = t0;
        dt = ((rand() % 7) + 1)*((rand() % 3) + 1);
        sum_dt += dt;
        deadline = t0 + dt*((rand() % 5)+2);
        fprintf(saida, "p%d %d %d %d\n", i, t0, dt, deadline);
    }

    fclose(saida);
    printf("Soma dos dt: %d\n", sum_dt);

    return 0;
}
