#include <sys/types.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h>  //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <stdio.h>
#include "struct.h"

//forma de correr o client
// ./client execute 100 -u "prog-a arg-1 (...) arg-n" output: task n received\n
//ou
// ./client execute 3000 -p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n" output: task n received\n
//ou
// ./client status output: Executing...\n Scheduled...\n Completed...\n

/*
Minfo
int id; // contador
int operaçao; //  1 == -u, 2 == -p, 3 == status
int time; // tempo esperado
int pid; // para entrar em contacto com o cliente
struct timeval start; //tempo de inicio atualizar o valor quando chega ao servidor
    //biblioteca do struct timeeval
    //long int tv_usec
    //The number of microseconds elapsed since the time given by the tv_sec member.
struct timeval end; // quando o filho termina de executar
    
char nome[512];
*/




int main(int argc, char **argv){   //argc: numero de argumentos presentes no argv; //argv: argumentos passados na inicializaçao do programa (ex: ./client execute -u "...") 
    if(argc<2){
        perror("Few arguments!");
        return -1;
    }
    char *args[20];
	char *string, *cmd, *tofree;
	int i=0;
	tofree = cmd = strdup(**argv);
	while((string = strsep(&cmd," "))!=NULL){
	   args[i]=string;
	   i++;
	}
	args[i]=NULL;
    free(tofree);                                      
    if (argc <   5){                        //5: porque o argv[0] = ./client
        if (argc == 2 &&  strcmp(argv[1],"status") == 0){         //1º evitar segmentation fault, 2º exemplo do enunciado ($ ./client status)
        //criar um processo novo e criar dois fifos(para ler e escrever no server), criar uma estrutura minfo e enviar;
        }else{
            perror("Few arguments!");
            return -1;
        }
    }else{ // mandar a informação e o servidor deve devolver o número do processo ao cliente para o cliente dar ao utilizador
        if(args[3] == "-u"){
            minfo info;
            info.time = atoi(args[2]);
            info.operaçao = args[3];
            info.nome = args[4];
        }else{ // args[3] == "-p"
            if(args[3]!= "-p"){
                perror("algo de errado não está certo, só vale -p ou -u.");
                return -1;
            }
        }
    }
    
    return 0;
}