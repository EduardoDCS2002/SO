#include "struct.h"


// forma de correr o orchestrator
// ./orchestrator output_folder parallel-tasks sched-policy
// sendo output_folder para onde vão os ficheiros das tarefas feitas executadas,
// parallel-tasks o máximo de tarefas a serem corridas em paralelo,
// a sched policy a política de escalonamento(vamos tentar fazer pelo menos 3)
//políticas de escalonamento: FCFS, SJF e custom
//sempre que um cliente entrar em contacto, incrementar 1 e retornar
// gettimeofday(&timeval, NULL);


//Funçao para calcular a diferença de tempo entre dois pontos (milisegundos)
void swapminfo(minfo *a, minfo *b){
    minfo tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}
long int time_diff(struct timeval *start, struct timeval *end) {
    return (1e+3 *(end->tv_sec - start->tv_sec)) + (1e-3 * (end->tv_usec - start->tv_usec));
}

char* escritanooutput(minfo mensagem){
    // id, pid, time, nome, execucao
    char* realoutput;
    sprintf(realoutput, "TASK (id %d, pid %d) - TIME %d miliseconds - COMMAND %s - OUTPUT %s\n"
    , mensagem.id, mensagem.pid, mensagem.time, mensagem.nome, mensagem.execucao);
}

//Funçao para implementar a politica de esclonamento
int sc_SJF(minfo (*fila)[], int N){
    int menor_tempo = (*fila)[0].time;
    int indice_menor_tempo = 0;

    for (int i = 1; i< N; i++){
        if((*fila)[i].time < menor_tempo){
            menor_tempo = (*fila)[i].time;
            indice_menor_tempo = i;
        }
    }
    for(int i=indice_menor_tempo;i<N-1;i++){
        swapminfo(&(*fila)[i],&(*fila)[i+1]);
    }
    indice_menor_tempo = N-1;
    return indice_menor_tempo;
}

//Funçao para implementar uma politica de escalonamento
int sc_CUSTOM(minfo (*fila)[], int N){ // se um gajo for ultrapassado, o mensagem.custom dele soma 1
    int menor_tempo = (*fila)[0].time; // se o mensagem.custom dele chegar a 3, ele é o escolhido.
    int indice_escolhido = 0;
    int ultrapassagem = (*fila)[0].custom;
    
    if(ultrapassagem < 3){
        for (int i = 1; i< N; i++){
            if((*fila)[i].custom >=3){
                indice_escolhido = i;
                break;
            }else{
                if((*fila)[i].time < menor_tempo){
                    menor_tempo = (*fila)[i].time;
                    indice_escolhido = i;
                }
            }
        }
    }
    for(int i=indice_escolhido;i<N-1;i++){
        swapminfo(&(*fila)[i],&(*fila)[i+1]);
    }
    indice_escolhido = N-1;
    return indice_escolhido;
}

int main(int argc, char * argv[]){
    printf("ai chegaste 1\n");
//Verifica os argumentos passados
    int N = atoi(argv[2]);
    char* sp = argv[3];
    if(((0 == (strcmp(sp, "FCFS"))) 
        && (0 == (strcmp(sp, "SJF"))) 
        && (0 == (strcmp(sp, "CUSTOM"))))){
        printf("deu true");
    }
    printf("ai chegaste 1\n");
    if((0 == strcmp(argv[1],OUTPUT))){
            if(N<0){
                if(0 == (strcmp(sp, "FCFS"))){
                    if(0 == (strcmp(sp, "SJF"))){
                        if(0 == (strcmp(sp, "CUSTOM"))){
                            perror("Pedro no input");
                            return -1;
                        }
                    }
                }
            }
    }
    printf("ai chegaste 2\n");
//Inicializa variáveis
    int countID = 1001; // id das mensagens
    int countPT = 0; // quantos processos estão a decorrer neste momento
    int sizefila = 10*N;
    minfo filaEspera[sizefila];
    minfo mensagem;
    
//Cria o FIFO do servidor
    if(mkfifo(SERVER, 0666)==-1){
		perror("erro ao criar o fifo");
		return -1;
	}

	printf("--- fifo do server e do escritor criado ---\n");
    int fifoserver_fd = open(SERVER, O_RDONLY);
	printf("--- fifo do server aberto ---\n");
    
//Loop principal do servidor para receber mensagens dos clientes
    while(1){
	    int read_bytes = read(fifoserver_fd, &mensagem, sizeof(struct minfo));
	    if (read_bytes != sizeof(struct minfo)){
	    	perror("struct recebida errada ou mal");
	    	mensagem.tipo = 16;
            mensagem.operacao = 16;
	    }
        
//Processa a mensagem recebida
        if(mensagem.tipo == 1){ //Se for uma tarefa a ser escrita no output
            countPT--;
            mensagem.time = time_diff(&mensagem.start, &mensagem.end);
            int pid = fork();
            if(pid==0){
                char *output = "/output-folder/output";
                int fdoutput = open(output, O_CREAT, O_WRONLY, O_TRUNC, 0666);
                char *realoutput = escritanooutput(mensagem);
                write(fdoutput,realoutput,strlen(realoutput));
            }
        
        }else{ //Se for uma tarefa a ser escalonada
	        printf("--- mensagem lida ---\n");
            if((mensagem.operacao == 1)||(mensagem.operacao == 0)){
                mensagem.id = countID;
                countID++;
                filaEspera[countPT] = mensagem;
                countPT++;
                int pf=0;
                if(strcmp(sp, "SJF")){
                    pf = sc_SJF(&filaEspera, countPT);
                }
                if(strcmp(sp, "CUSTOM")){
                    pf = sc_CUSTOM(&filaEspera, countPT);
                }
                mensagem = filaEspera[pf];
            }
            if(mensagem.operacao == 2){ //status -- cat do output
                int pid = fork();
                
                if(pid == 0){ // o filho do filho vai executar e o filho vai mandar o
                    int pipes[2]; // output diretamente para o cliente
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
                    read(1, &outputstatus, strlen(outputstatus));

                    char fifoc_name[30];
	                sprintf(fifoc_name, CLIENT "%d", mensagem.pid);
                    int fifocliente_fd = open(fifoc_name, O_WRONLY);
	                write(fifocliente_fd, &outputstatus, strlen(outputstatus));
                    _exit(0);
                }
            }
            gettimeofday(&mensagem.start, NULL);

//Outras operaçoes para executar tarefas
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
                    read(pipes[1], &mensagem.execucao, strlen(mensagem.execucao));
                    mensagem.tipo = 1;
                    gettimeofday(&mensagem.end, NULL);

                    int fifoserver_fd = open(SERVER, O_WRONLY);
                    write(fifoserver_fd,&mensagem, sizeof(struct minfo));
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
                    read(1,&mensagem.execucao,strlen(mensagem.execucao));
                    mensagem.tipo = 1;
                    gettimeofday(&mensagem.end, NULL);

                    int fifoserver_fd = open(SERVER, O_WRONLY);
                    write(fifoserver_fd,&mensagem, sizeof(struct minfo));
	                close(fifoserver_fd);
                    _exit(0);

                }
            }
        }
    }
    return 0;
}