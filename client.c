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
    
char nome[300];
*/




int main(int argc, char **argv){   //argc: numero de argumentos presentes no argv; //argv: argumentos passados na inicializaçao do programa (ex: ./client execute -u "...") 
    if(argc!=2 || argc!=5){
        perror("Wrong number of arguments!");
        return -1;
    }
    minfo mensagem;
    mensagem.pid = getpid();
    mensagem.tipo = 0;
    if (argc == 2 &&  strcmp(argv[1],"status") == 0){
        mensagem.operaçao = 2;
        mensagem.time = 0;
        mensagem.nome = "status";
    }else{ // para ter a certeza que não há erros no input
        if(!(strcmp(argv[1], "execute")) ||  
        !(strcmp(argv[3],"-u") || strcmp(argv[3], "-p")) ||
        !(atoi(argv[2]>0))){ //verificar o argv[4] no server
            perror("Arguments don't have the correct values!");
            return -1;
        }
        mensagem.time = argv[2];
        mensagem.operaçao = strcmp(argv[3],"-u");
        mensagem.nome = argv[4];
    }

    char fifoc_name[30];
	sprintf(fifoc_name, CLIENT "%d", mensagem.pid);
	if(mkfifo(fifoc_name, 0666)==-1){
		perror("erro ao criar o fifo");
		return -1;
	}

    //printf("--- fifo do clinte criado ---\n");
	int fifoserver_fd = open(SERVER, O_WRONLY);
	//printf("--- fifo do server aberto ---\n");
    write(fifoserver_fd,&mensagem, sizeof(mensagem));
	//printf("--- mensagem mandada para o server ---\n");
	close(fifoserver_fd);

    int fifocliente_fd = open(fifoc_name, O_RDONLY);
	//printf("--- fifo do clinte aberto ---\n");
	read(fifocliente_fd, &mensagem, sizeof(mensagem));
	//printf("--- mensagem lida ---\n");

    unlink(fifoc_name);

    if(mensagem.operaçao != 2){
        char* output;
        sprintf(output, "Task %d received",mensagem.id);

        write(1,output, sizeof(output));
    }else{
        //imprimir o status bonitinho ou o server faz tudo?
    }
    return 0;
}