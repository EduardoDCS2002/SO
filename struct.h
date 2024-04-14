#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h>  //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <stdio.h>

#define SERVER "fifo_server" 
#define CLIENT "fifo_client"
#define sizeExecute 300
#define OUTPUT "output-folder"

typedef struct minfo{
    int id; //identificador do comando que é dado quando o servidor o recebe, "contador"
    int tipo; // 0 se for mensagem normal 
    // e 1 se for um filho do server a mandar para o server
    int operaçao;//  0 == -p, 1 == -u, 2 == status
    int time; // tempo esperado, exemplo : 100 ms
    int pid; // para entrar em contacto com o cliente
    struct timeval start; //tempo de inicio atualizar o valor quando chega ao servidor
    //biblioteca do struct timeeval
    //long int tv_usec
    //The number of microseconds elapsed since the time given by the tv_sec member.
    struct timeval end; // quando o filho termina de executar
    
    char *nome; 
}minfo;

/*
Lista no servidor vai ser uma lista de minfo

Elemento lista[2048]; //numero 2^n

Filho termina de executar e comunica ao pai  -> pai escreve para no mesmo ficheiro (escrever o que esta no enunciado)
*/

// Exemplo de outro algoritmo sem ser o FIFO (First in first out - executar por ordem de chegada), ter a lista ordenada por os que demoram menos tempo a executar 

// FIFO  1º 100 ms 2º 3000 ms 3º 5 ms primeiro a chegar primeiro a executar

// Outra maneira ordenar pelo ordem crescente do tempo 1º 5 ms 2º 100 ms 3º 3000 ms tambem pode ser procurar o mais pequeno adicionas a 
// lista no fim e quando for para executar um procurar o que tem menor valor de tempo