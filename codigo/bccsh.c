#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>

#include <readline/history.h>
#include <readline/readline.h>

#define SIZE_CUR_DIR 100 

// Variáveis (globais?) usuário e diretório atual ====
char cur_dir[SIZE_CUR_DIR]; 
char *cur_user;

/** Função type_prompt
 * Escreve na tela o prompt com o usuário e diretório atual
 */
void type_prompt() { 
    // struct passwd * p;

    // p = getpwuid(getuid());
    // cur_user = p->pw_name;

    cur_user = getenv("USER");

    getcwd(cur_dir, sizeof(cur_dir));

    printf("{%s@%s} ", cur_user, cur_dir);
}

/** Função read_command
 * Usa GNU readline e GNU history para ler um comando do terminal 
 * e retornar esse comando e os parâmetros indicados.
 */ 
void read_command(char **command, char parameters[][100]) {
    char prompt_line[500];

    cur_user = getenv("USER");
    getcwd(cur_dir, sizeof(cur_dir));
    sprintf(prompt_line, "{%s@%s} ", cur_user, cur_dir);

    *command = readline(prompt_line);
    add_history(*command); 

    int i = 0;

    while ((*command)[i] != ' ' && (*command)[i] != '\0') { i++; }
    if ((*command)[i] == ' ') { 
        (*command)[i] = '\0'; i++;
    }


    for (int j = 0; j < 20; parameters[j][0] = '\0', j++);

    int j = 0, k = 0;
    for (; (*command)[i] != '\0'; i++) {
        if ((*command)[i] == ' ') {
            parameters[j][k] = '\0';
            k = 0;
            j++;
        } else {
            parameters[j][k] = (*command)[i];
            k++;
        }
    }
    parameters[j][k] = '\0';
}

void shell_control() {
    // Comando e parâmetro lidos no terminal
    char *command;
    char parameters[20][100];
    // Variável devolvida pelo método waitpid
    int status;

    while (1) {
        // type_prompt();
        read_command(&command, parameters);

        /* printf("command = '%s'\n", command);
        for (int j = 0; j < 20; j++) 
            if (parameters[j][0]) printf("param = '%s'\n", parameters[j]);
        */


        // ifzão dos syscall

        /*
         if (!strcmp(command, "mkdir")) {
        }

        if (fork() != 0) {
            // Código do processo pai
            waitpid(-1, &status, 0);
        } else {
            // Código do processo filho
            execve(command, parameters, 0);
        } */
    }
}

int main(int argc, char const *argv[])
{
    char c;

    shell_control();

    scanf("%c", &c);
    return 0;
}
