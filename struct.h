#include <time.h>

typedef struct minfo{
    int id; //identificador do comando que é dado quando o servidor o recebe, "contador"
    int operaçao;//  1 == -u, 2 == -p, 3 == status
    int time; // tempo esperado, exemplo : 100 ms
    int pid; // para entrar em contacto com o cliente
    struct timeval start; //tempo de inicio atualizar o valor quando chega ao servidor
    //biblioteca do struct timeeval
    //long int tv_usec
    //The number of microseconds elapsed since the time given by the tv_sec member.
    struct timeval end; // quando o filho termina de executar
    
    char nome[512]; 
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