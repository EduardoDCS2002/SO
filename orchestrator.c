#include "struct.h"


// forma de correr o orchestrator
// ->/orchestrator output_folder parallel-tasks sched-policy
// sendo output_folder para onde vão os ficheiros das tarefas feitas executadas,
// parallel-tasks o máximo de tarefas a serem corridas em paralelo,
// a sched policy a política de escalonamento(vamos tentar fazer pelo menos 3)
//políticas de escalonamento: FCFS, SJF e custom
//sempre que um cliente entrar em contacto, incrementar 1 e retornar
// gettimeofday(&timeval, NULL);
int mensagemrecebida(int id, minfo queue[], int tam){
    for(int i=0; i<tam; i++){
        if(queue[i]->id == id){
            return 1;
        }
    }
    return 0;
}
void swapminfo(minfo a, minfo b){
    minfo tmp;
    *tmp = *a;
    *a = *b;
    *b = *tmp;
}
long int time_diff(struct timeval *start, struct timeval *end) {
    return (1e+3 *(end->tv_sec - start->tv_sec)) + (1e-3 * (end->tv_usec - start->tv_usec));
}

char* escritanooutput(minfo mensagem){
    // id, pid, time, nome, execucao
    char* realoutput;
    sprintf(realoutput, "TASK (id %d, pid %d) - TIME %d miliseconds - COMMAND %s - OUTPUT %s\n"
    , mensagem->id, mensagem->pid, mensagem->time, mensagem->nome, mensagem->execucao);
    return realoutput;
}

//Funçao para implementar a politica de esclonamento
int sc_SJF(minfo (*fila)[], int N){
    int menor_tempo = (*fila)[0]->time;
    int indice_menor_tempo = 0;

    for (int i = 1; i< N; i++){
        if((*fila)[i]->time < menor_tempo){
            menor_tempo = (*fila)[i]->time;
            indice_menor_tempo = i;
        }
    }
    for(int i=indice_menor_tempo;i<N-1;i++){
        swapminfo((*fila)[i],(*fila)[i+1]);
    }
    indice_menor_tempo = N-1;
    return indice_menor_tempo;
}

//Funçao para implementar uma politica de escalonamento
int sc_CUSTOM(minfo (*fila)[], int N){ // se um gajo for ultrapassado, o mensagem->custom dele soma 1
    int menor_tempo = (*fila)[0]->time; // se o mensagem->custom dele chegar a 3, ele é o escolhido->
    int indice_escolhido = 0;
    for (int i = 0; i< N; i++){
        if((*fila)[i]->custom >=3){
            indice_escolhido = i;
            break;
        }else{
            if((*fila)[i]->time < menor_tempo){
                menor_tempo = (*fila)[i]->time;
                indice_escolhido = i;
            }
        }
    }
    for(int i=indice_escolhido;i<N-1;i++){
        swapminfo((*fila)[i],(*fila)[i+1]);
    }
    indice_escolhido = N-1;
    return indice_escolhido;
}

int main(int argc, char * argv[]){
    // Verifica os argumentos passados
    int N = atoi(argv[2]);
    char* sp = argv[3];
    if((0 == strcmp(argv[1],OUTPUT))){
        perror("erro no input");
        return -1;
    }
    if(N<0){
        perror("erro no input");
        return -1;
    }    
    if(0 == (strcmp(sp, "FCFS"))){
        if(0 == (strcmp(sp, "SJF"))){
            if(0 == (strcmp(sp, "CUSTOM"))){
                perror("erro no input");
                return -1;
            }
        }
    }
//Inicializa variáveis
    int countID = 1001; // id das mensagens
    int countPT = 0; // quantos processos estão a decorrer neste momento
    int countfila = 0; // quantos processos estão na fila
    minfo filaEspera[10*N];
    minfo mensagem = malloc(sizeof(struct minfo));
    
//Cria o FIFO do servidor
    if(mkfifo(SERVER, 0666)==-1){
		perror("erro ao criar o fifo");
		return -1;
	}

	printf("--- fifo do server e do escritor criado ---\n");
    int fifoserver_fd = open(SERVER, O_RDONLY);
	printf("--- fifo do server aberto ---\n");
    
    while(1){
	    int read_bytes = read(fifoserver_fd, mensagem, sizeof(struct minfo)); // não precisas de verificar se a mensagem
                                                            // está correta porque isso é visto no cliente
        //Processa a mensagem recebida
        if(mensagem->tipo == 1){ //Se for uma tarefa a ser escrita no output
            countPT--;
            int pid = fork();
            if(pid==0){
                char *output = "/output-folder/output";
                int fdoutput = open(output, O_CREAT, O_WRONLY, 0666);
                char *realoutput = escritanooutput(mensagem);
                write(fdoutput,realoutput,strlen(realoutput));
                _exit(0);
            }
            if((countfila>0) && (countPT<N)){ // manda fazer um novo processo se tiver processos para fazer
                int pf=0;
                if(0 == strcmp(sp, "SJF")){
                    pf = sc_SJF(&filaEspera, countfila);
                }
                if(0 == strcmp(sp, "CUSTOM")){
                    pf = sc_CUSTOM(&filaEspera, countfila);
                }
                mensagem = filaEspera[pf];
                countfila--;
                countPT++;
                gettimeofday(&(mensagem->start), NULL);
                if(mensagem->operacao == 1){ // -u é 1 comando só
                    int pid = fork();
                    if(pid==0){
                        int pipes[2];
                        pipe(pipes);
                        int pidfilho = fork();
                        if(pidfilho==0){
                            close(pipes[0]);
                            dup2(pipes[1],1);
                            close(pipes[1]);
                            char *comando;
                            comando = strdup(mensagem->nome);
                            char *nome = strsep(&comando, " ");
                            execlp(nome, nome, comando, NULL);
                            _exit(0);
                        }
                        close(pipes[1]);
                        read(pipes[0], mensagem->execucao, 300);
                        mensagem->tipo = 1;
                        gettimeofday(&(mensagem->end), NULL);
                        mensagem->time = time_diff(&mensagem->start, &mensagem->end);

                        int fifoserver_fd = open(SERVER, O_WRONLY);
                        write(fifoserver_fd,mensagem, sizeof(struct minfo));
                        close(fifoserver_fd);
                        _exit(0);
                    }
                }
                if(mensagem->operacao == 0){ // -p vários comandos
                    int pidpai = fork();
                    if(pidpai==0){
                        char* exec_comandos[10];
                        char *string, *cmd;
                        int rN=0;
                        cmd = strdup(mensagem->nome);
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
                                close(pipes[i][1]);
                            }
                            if(i==rN-1){
                                int pidfim = fork();
                                if(pidfim == 0){
                                    dup2(pipes[i-1][0],0);
                                    close(pipes[i-1][0]);
                                    char *comando;
                                    comando = strdup(exec_comandos[i]);
                                    char *nome = strsep(&comando, " ");
                                    execlp(nome, nome, comando, NULL);
                                    _exit(0);
                                }
                                close(pipes[i-1][0]);
                            }else{
                                pipe(pipes[i]);
                                int pidmeio = fork();
                                if(pidmeio == 0){
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
                        read(1,mensagem->execucao,300);
                        mensagem->tipo = 1;
                        gettimeofday(&(mensagem->end), NULL);
                        mensagem->time = time_diff(&mensagem->start, &mensagem->end);

                        int fifoserver_fd = open(SERVER, O_WRONLY);
                        write(fifoserver_fd,mensagem, sizeof(struct minfo));
                        close(fifoserver_fd);
                        _exit(0);

                    }
                }
            }
        
        }else{ //Se for uma tarefa a ser escalonada
            if(0 == mensagemrecebida(mensagem->id, filaEspera, countfila)){
                printf("--- mensagem lida ---\n");
                if(mensagem->operacao == 2){ //status -- cat do output
                    int pid = fork();
                    
                    if(pid == 0){ // o filho do filho vai executar e o filho vai mandar o
                        int pipes[2]; // output diretamente para o cliente
                        pipe(pipes);
                        int pidfilho = fork();
                        if(pidfilho == 0){
                            close(pipes[0]);
                            dup2(pipes[1], 1);
                            close(pipes[1]); 
                            execlp("cat", "cat", "/output-folder/output", NULL);
                            _exit(0);
                        }
                        close(pipes[1]);
                        char outputstatus[4096];
                        read(pipes[0], &outputstatus, 4096);

                        char fifoc_name[30];
                        sprintf(fifoc_name, CLIENT "%d", mensagem->pid);
                        int fifocliente_fd = open(fifoc_name, O_WRONLY);
                        write(fifocliente_fd, &outputstatus, strlen(outputstatus));
                        _exit(0);
                    }
                }else{
                    if((mensagem->operacao == 1)||(mensagem->operacao == 0)){
                        mensagem->id = countID;
                        countID++;
                        filaEspera[countfila] = mensagem;
                        countfila++;
                        if(countfila==1){
                            countfila--;
                            gettimeofday(&(mensagem->start), NULL);
                            if(mensagem->operacao == 1){ // -u é 1 comando só
                                int pid = fork();
                                if(pid==0){
                                    int pipes[2];
                                    pipe(pipes);
                                    int pidfilho = fork();
                                    if(pidfilho==0){
                                        close(pipes[0]);
                                        dup2(pipes[1],1);
                                        close(pipes[1]);
                                        char *comando;
                                        comando = strdup(mensagem->nome);
                                        char *nome = strsep(&comando, " ");
                                        execlp(nome, nome, comando, NULL);
                                        _exit(0);
                                    }
                                    close(pipes[1]);
                                    read(pipes[0], mensagem->execucao, 300);
                                    mensagem->tipo = 1;
                                    gettimeofday(&(mensagem->end), NULL);
                                    mensagem->time = time_diff(&mensagem->start, &mensagem->end);

                                    int fifoserver_fd = open(SERVER, O_WRONLY);
                                    write(fifoserver_fd,mensagem, sizeof(struct minfo));
                                    close(fifoserver_fd);
                                    _exit(0);
                                }
                            }
                            if(mensagem->operacao == 0){ // -p vários comandos
                                int pidpai = fork();
                                if(pidpai==0){
                                    char* exec_comandos[10];
                                    char *string, *cmd;
                                    int rN=0;
                                    cmd = strdup(mensagem->nome);
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
                                            close(pipes[i][1]);
                                        }
                                        if(i==rN-1){
                                            int pidfim = fork();
                                            if(pidfim == 0){
                                                dup2(pipes[i-1][0],0);
                                                close(pipes[i-1][0]);
                                                char *comando;
                                                comando = strdup(exec_comandos[i]);
                                                char *nome = strsep(&comando, " ");
                                                execlp(nome, nome, comando, NULL);
                                                _exit(0);
                                            }
                                            close(pipes[i-1][0]);
                                        }else{
                                            pipe(pipes[i]);
                                            int pidmeio = fork();
                                            if(pidmeio == 0){
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
                                    read(1,mensagem->execucao,300);
                                    mensagem->tipo = 1;
                                    gettimeofday(&(mensagem->end), NULL);
                                    mensagem->time = time_diff(&mensagem->start, &mensagem->end);

                                    int fifoserver_fd = open(SERVER, O_WRONLY);
                                    write(fifoserver_fd,mensagem, sizeof(struct minfo));
                                    close(fifoserver_fd);
                                    _exit(0);

                                }
                            }
                        }
                    }    
                }
            }
        }
    }
    return 0;
}