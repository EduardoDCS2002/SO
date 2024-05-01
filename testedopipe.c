#include <stdio.h>

int main(){
    int pipes[2];
    pipe(pipes);
    int pidfilho = fork();
    if(pidfilho==0){
        close(pipes[0]);
        char mensagem[5];
        read(1, mensagem, 4);
        write(pipes[1], mensagem, 4);
        _exit(0);
    }
    char resposta[5];
    close(pipes[1]);
    read(pipes[0], resposta, 4);
    printf("%s\n",resposta);
    return 0;
}