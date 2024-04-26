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
    //The number of microseconds elapsed since the time given by the tv_sec member->
struct timeval end; // quando o filho termina de executar
    
char nome[300];
*/




int main(int argc, char **argv){   //argc: numero de argumentos presentes no argv; //argv: argumentos passados na inicializaçao do programa (ex: ->/client execute -u "->->->") 

//Verifica se o número de argumentos está correto
    if(argc!=2 && argc!=5){
        perror("Wrong number of arguments!");
        return -1;
    }
    minfo mensagem = malloc(sizeof(struct minfo));
//Preenche a estrutura "mensagem" correspondente com o input
    mensagem->pid = getpid();
    mensagem->tipo = 0;
    mensagem->custom = 0;
    
    if (argc == 2 &&  (strcmp(argv[1],"status") == 0)){
        mensagem->operacao = 2;
        mensagem->time = 0;

    }else{ // para ter a certeza que não há erros no input
        if(!(0 == strcmp(argv[1], "execute")) &&  
        (!(0 == strcmp(argv[3],"-u")) || !(0 == strcmp(argv[3], "-p"))) &&
        atoi(argv[2])<=0){ //verificar o argv[4] no server
            
            perror("Arguments don't have the correct values!");
            return -1;
        }
        
        mensagem->time = atoi(argv[2]);
        mensagem->operacao = (1 == strcmp(argv[3],"-u"));
        sprintf(mensagem->nome, argv[4]);
    }

//Cria o FIFO do cliente para depois ler
    char fifoc_name[30];
	sprintf(fifoc_name, CLIENT "%d", mensagem->pid);
	
    if(mkfifo(fifoc_name, 0666)==-1){
		perror("erro ao criar o fifo");
		return -1;
	}

	int fifoserver_fd = open(SERVER, O_WRONLY);
	
//Escreve a mensagem no FIFO do servidor
    write(fifoserver_fd,mensagem, sizeof(struct minfo));
	close(fifoserver_fd);

//Abre o FIFO do cliente para leitura
    int fifocliente_fd = open(fifoc_name, O_RDONLY);

//Lê e processa a resposta do servidor
    if(mensagem->operacao != 2){
        read(fifocliente_fd, mensagem, sizeof(struct minfo));
        char* output;
        sprintf(output, "Task (id %d, pid %d) received", mensagem->id, mensagem->pid);

        write(1,output, strlen(output));
    
    }else{
        char statusoutput[4096];
        int bytes_read;
        while(bytes_read>0){
            bytes_read = read(fifocliente_fd, &statusoutput, 128);
        }
        write(1,statusoutput, strlen(statusoutput));
    }
    
    unlink(fifoc_name);
    return 0;
}