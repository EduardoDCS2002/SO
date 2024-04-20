#include "struct.h"


// forma de correr o orchestrator
// ./orchestrator output_folder parallel-tasks sched-policy
// sendo output_folder para onde vão os ficheiros das tarefas feitas executadas,
// parallel-tasks o máximo de tarefas a serem corridas em paralelo,
// a sched policy a política de escalonamento(vamos tentar fazer pelo menos 3)
//políticas de escalonamento: FCFS, SJF e custom
//sempre que um cliente entrar em contacto, incrementar 1 e retornar
// gettimeofday(&timeval, NULL);

long int time_diff(struct timeval *start, struct timeval *end) {
    return (1e+3 *(end->tv_sec - start->tv_sec)) + (1e-3 * (end->tv_usec - start->tv_usec));
}

char* convertermensagem(minfo mensagem){ // quando isto é usado o mensagem.nome é o output
    char* realoutput;

    return realoutput;
}

int sc_SJF(minfo fila[], int N){
    int r;

    return r;
}
int sc_CUSTOM(minfo fila[], int N){
    int r;

    return r;
}

int main(int argc, char * argv[]){
    int N = atoi(argv[2]);
    char* sp = argv[3];
    if(!strcmp(argv[1],OUTPUT) 
    || !(0>N) 
    || !(strcmp(sp, "FCFS") 
        || strcmp(sp, "SJF") 
        || strcmp(sp, "CUSTOM"))){
            perror("erro no input");
            return -1;
        }
    int countID = 1001; // id das mensagens
    int countPT = 0; // quantos processos estão a decorrer neste momento
    int sizefila = 10*N;
    minfo filaEspera[sizefila];
    
    minfo mensagem;
    
    if(mkfifo(SERVER,0666)==-1){
		perror("erro ao criar o fifo");
		return -1;
	}

	printf("--- fifo do server e do escritor criado ---\n");
    int fifoserver_fd = open(SERVER, O_RDONLY);
	printf("--- fifo do server aberto ---\n");
    while(1){
	    int read_bytes = read(fifoserver_fd, &mensagem, sizeof(mensagem));
	    if (read_bytes != sizeof(mensagem)){
	    	perror("struct recebida errada ou mal");
	    	break;
	    }
        if(mensagem.tipo == 1){ // escrever no output
            int pid = fork();
            if(pid==0){
                char *output = "/output-folder/output";
                int fdoutput = open(output, O_CREAT, O_WRONLY, O_TRUNC, 0666);
                char *realoutput = convertermensagem(mensagem);
                write(fdoutput,realoutput,sizeof(realoutput));

            }
        }else{
	        printf("--- mensagem lida ---\n");
            mensagem.id = countID;
            countID++;
            filaEspera[countPT] = mensagem;
            countPT++;
            int pf=0;
            if(strcmp(sp, "SJF")){
                pf = sc_SJF(filaEspera, countPT);
            }
            if(strcmp(sp, "CUSTOM")){
                pf = sc_CUSTOM(filaEspera, countPT);
            }
            mensagem = filaEspera[pf];
            
            if(mensagem.operacao == 2){ //status -- cat do output
                int pid = fork();
                if(pid == 0){
                    int pipes[2];
                    pipe(pipes);
                    int pidfilho = fork();
                    if(pidfilho == 0){
                        close(pipes[1]);
                        dup2(pipes[0], 0);
                        close(pipes[0]); 
                        execlp("cat", "cat", "/output-folder/output", NULL);
                        _exit(0);
                    }
                    char outputstatus[512];
                    read(1, &outputstatus, sizeof(outputstatus));

                    char fifoc_name[30];
	                sprintf(fifoc_name, CLIENT "%d", mensagem.pid);
                    int fifocliente_fd = open(fifoc_name, O_WRONLY);
	                write(fifocliente_fd, &outputstatus, sizeof(outputstatus));
                    _exit(0);
                }
            }
            if(mensagem.operacao == 1){ // -u é 1 comando só
                int pid = fork();
                if(pid==0){
                    int pipes[2];
                    pipe(pipes);
                    int pidfilho = fork();
                    if(pidfilho==0){
                        close(pipes[1]);
                        dup2(pipes[0],0);
                        close(pipes[0]);
                        char *comando;
                        comando = strdup(mensagem.nome);
                        char *nome = strsep(&comando, " ");
	                    execlp(nome, nome, comando, NULL);
                        _exit(0);
                    }
                    close(pipes[0]);
                    read(pipes[1], &mensagem.nome, 300);
                    mensagem.tipo = 1;

                    int fifoserver_fd = open(SERVER, O_WRONLY);
                    write(fifoserver_fd,&mensagem, sizeof(mensagem));
	                close(fifoserver_fd);
                    _exit(0);
                }
            }
            if(mensagem.operacao == 0){ // -p vários comandos
                int pidpai = fork();
                if(pidpai==0){
                    char *exec_comandos[N];
	                char *string, *cmd, *tofree;
	                int rN=0;
	                cmd = strdup(mensagem.nome);
	                while((string = strsep(&cmd,"|"))!=NULL){
	                    exec_comandos[rN]=string;
	                    rN++;
	                }
	                exec_comandos[rN]=NULL;
                    int pipes[rN-1][2];
                    for(int i=0;i<rN;i++){
                        if(i==0){
                            pipe(pipes[i]);
                            int pid0 = fork();
                            if(pid0==0){
                                close(pipes[i][0]);
                                dup2(pipes[i][1],1);
                                close(pipes[i][1]);
                                char *comando;
                                comando = strdup(exec_comandos[i]);
                                char *nome = strsep(&comando, " ");
	                            execlp(nome, nome, comando, NULL);
                                _exit(0);
                            } 
                        }
                        if(i==rN-1){
                            int pidmeio = fork();
                            if(pidmeio == 0){
                                dup2(pipes[i-1][0],0);
                                close(pipes[i-1][0]);
                                char *comando;
                                comando = strdup(exec_comandos[i]);
                                char *nome = strsep(&comando, " ");
	                            execlp(nome, nome, comando, NULL);
                                _exit(0);
                            }
                            close(pipes[i-1][1]);
                        }else{
                            pipe(pipes[i]);
                            int pid = fork();
                            if(pid == 0){
                                close(pipes[i][0]);
                                dup2(pipes[i-1][0],0);
                                close(pipes[i-1][0]);
                                dup2(pipes[i][1],1);
                                close(pipes[i][1]);
                                char *comando;
                                comando = strdup(exec_comandos[i]);
                                char *nome = strsep(&comando, " ");
	                            execlp(nome, nome, comando, NULL);
                                _exit(0);
                            }
                            close(pipes[i-1][0]);
                            close(pipes[i][1]);
                        }
                    }
                    read(1,&mensagem.nome,300);
                    mensagem.tipo = 1;

                    int fifoserver_fd = open(SERVER, O_WRONLY);
                    write(fifoserver_fd,&mensagem, sizeof(mensagem));
	                close(fifoserver_fd);
                    _exit(0);

                }
            }
        }
    }
    return 0;
}