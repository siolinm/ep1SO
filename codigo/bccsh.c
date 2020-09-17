#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>

#include <readline/history.h>
#include <readline/readline.h>

#define SIZE_CUR_DIR 100 

// Variáveis (globais?) usuário e diretório atual ====
char cur_dir[SIZE_CUR_DIR]; 
char *cur_user;

/** Função read_command
 * Usa GNU readline e GNU history para ler um comando do terminal 
 * e retornar esse comando e os parâmetros indicados.
 */ 
void read_command(char **command, char **parameters) {
    char prompt_line[500];

    // Imprimindo o prompt
    cur_user = getenv("USER");
    getcwd(cur_dir, sizeof(cur_dir));
    sprintf(prompt_line, "{%s@%s} ", cur_user, cur_dir);

    // Lendo comando do shell
    *command = readline(prompt_line);
    add_history(*command);

    // Retornando o comando e os parâmetros
    int i = 0;

    while ((*command)[i] != ' ' && (*command)[i] != '\0') { i++; }

    for (int j = 0; j < 20; parameters[j] = NULL, j++);

    for (int j = 0; (*command)[i] != '\0'; i++) {
        if ((*command)[i] == ' ') {
            parameters[j] = *command+i+1;
            (*command)[i] = '\0';
            j++;
        }
    }
}

void shell_control() {
    // Comando e parâmetro lidos no terminal
    char *command;
    char *parameters[20];
    // Variável devolvida pelo método waitpid
    int status;

    while (1) {
        read_command(&command, parameters);

        // ifzão dos syscall
        if (!strcmp(command, "mkdir")) {
           mkdir(parameters[0], S_IRWXU);
        } else if (!strcmp(command, "exit")) {
            rl_clear_history();
            exit(0);
        } else if (!strcmp(command, "kill")) {
            // Tratar o comando e pegar os parâmetros e PID
            pid_t pid = atoi(parameters[1]);
            kill(pid, -atoi(parameters[0])); 
        } else if (!strcmp(command, "ln") && !strcmp(parameters[0], "-s")) {
           symlink(parameters[1], parameters[2]); 
        }

        if (fork() != 0) {
            // Código do processo pai
            waitpid(-1, &status, 0);
        } else {
            // Código do processo filho
            execve(command, parameters, 0);
        } 
        free(command);
    }
}

int main(int argc, char const *argv[]) {
    shell_control();

    return 0;
}
