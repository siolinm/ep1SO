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

/* variáveis (globais?) usuário e diretório atual ==== */
char cur_dir[SIZE_CUR_DIR]; 
char *cur_user;

/** Função read_command(char **command, char **parameters)
 * Usa GNU readline e GNU history para ler um comando do terminal 
 * e retornar esse comando em command e os parâmetros indicados em parameters.
 */ 
void read_command(char **command, char **parameters){
    char prompt_line[500];

    /* imprimindo o prompt */
    cur_user = getenv("USER");
    getcwd(cur_dir, sizeof(cur_dir));
    sprintf(prompt_line, "{%s@%s} ", cur_user, cur_dir);

    /* lendo comando do shell */
    *command = readline(prompt_line);
    add_history(*command);

    /* retornando o comando e os parâmetros, reaproveitando a memória que já foi alocada por readline */
    int i = 0;
    
    if(*command != NULL){
        while((*command)[i] != ' ' && (*command)[i] != '\0') { i++; }

        for(int j = 0; j < 20; parameters[j] = NULL, j++);
        
        parameters[0] = *command;

        for(int j = 1; (*command)[i] != '\0'; i++){
            if ((*command)[i] == ' ') {
                parameters[j] = *command+i+1;
                (*command)[i] = '\0';
                j++;
            }
        }
    }
}

int main(int argc, char const *argv[]) {
    /* comando e parâmetro lidos no terminal */
    char *command;
    char *parameters[20];
    /* variável devolvida pelo método waitpid */
    int status;

    while (1) {
        read_command(&command, parameters);

        /* if para syscalls */
        if(command == NULL){
            printf("\n");
            rl_clear_history();
            exit(0);
        }
        else if(!strcmp(command, "mkdir")){
           mkdir(parameters[1], S_IRWXU);
        }
        else if(!strcmp(command, "kill")){
            /* tratar o comando e pegar os parâmetros e PID */
            pid_t pid = atoi(parameters[2]);
            kill(pid, -atoi(parameters[1]));
        }
        else if(!strcmp(command, "ln") && !strcmp(parameters[1], "-s")){            
            symlink(parameters[2], parameters[3]); 
        }
        else{
            if(fork() != 0){
                /* código do processo pai */
                waitpid(-1, &status, 0);
            }
            else{
                /* código do processo filho */            
                execve(command, parameters, NULL);
                exit(0);
            }
        }

        free(command);
    }

    return 0;
}
