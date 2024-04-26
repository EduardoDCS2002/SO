#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h>  //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>


#define SERVER "fifo_server" 
#define CLIENT "fifo_client"
#define sizeExecute 300
#define OUTPUT "/output-folder/"

typedef struct minfo{
    int id;
    int tipo; // 0 se for mensagem normal 
    // e 1 se for um filho do server a mandar para o server
    int operacao;//  0 == -p, 1 == -u, 2 == status
    int time;
    int pid; // para entrar em contacto com o cliente
    int custom; // para a política de escalonamento CUSTOM
    struct timeval start; //tempo de inicio atualizar o valor quando chega ao servidor
    struct timeval end; // quando o filho termina de executar
    char execucao[300];
    char nome[300];
}*minfo;

/*
Lista no servidor vai ser uma lista de minfo

Elemento lista[2048]; //numero 2^n

Filho termina de executar e comunica ao pai  -> pai escreve para no mesmo ficheiro (escrever o que esta no enunciado)
*/

// Exemplo de outro algoritmo sem ser o FIFO (First in first out - executar por ordem de chegada), ter a lista ordenada por os que demoram menos tempo a executar 

// FIFO  1º 100 ms 2º 3000 ms 3º 5 ms primeiro a chegar primeiro a executar

// Outra maneira ordenar pelo ordem crescente do tempo 1º 5 ms 2º 100 ms 3º 3000 ms tambem pode ser procurar o mais pequeno adicionas a 
// lista no fim e quando for para executar um procurar o que tem menor valor de tempo