#include "struct.h"

// forma de correr o orchestrator
// ./orchestrator output_folder parallel-tasks sched-policy
// sendo output_folder para onde vão os ficheiros das tarefas feitas executadas,
// parallel-tasks o máximo de tarefas a serem corridas em paralelo,
// a sched policy a política de escalonamento(vamos tentar fazer pelo menos 3)
//políticas de escalonamento: FCFS, SJF e custom
//sempre que um cliente entrar em contacto, incrementar 1 e retornar
int main(int argc, char * argv[]){
    if(!strcmp(argv[1],OUTPUT) 
    || 0>=atoi(argv[2]) 
    || (strcmp(argv[3], "FCFS") 
        || strcmp(argv[3], "SJF") 
        || strcmp(argv[3], "CUSTOM"))){
            perror("erro no input");
            return -1;
        }
    int countID = 1001;
    int countPT = 0;
    minfo filaEspera[atoi(argv[2])];
    
    minfo mensagem;
    
    if(mkfifo(SERVER,0666)==-1){
		perror("erro ao criar o fifo");
		return -1;
	}
	printf("--- fifo do server criado ---\n");
    int fifoserver_fd = open(SERVER, O_RDONLY);
	printf("--- fifo do server aberto ---\n");
    while(1){
	    int read_bytes = read(fifoserver_fd, &mensagem, sizeof(mensagem));
	    if (read_bytes != sizeof(mensagem)){
	    	perror("struct recebida errada ou mal");
	    	return -1;
	    }
	    printf("--- lida a mensagem ---\n");

        mensagem.id = countID;
        countID++;

        // se for status ainda não sei como vai ser
        // else criar filho para executar a mensagem
    }

    return 0;
}