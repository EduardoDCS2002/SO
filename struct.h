typedef struct comicacaoServer{
    int idComando; //identificador do comando que é dado quando o servidor o recebe "contador"
    int operaçao;// nao precisa de ser um inteiro(pode ser uma string)!! 1 significa que é um execute -u, 2 significa execute -p, 3 - um status ect....
    int tempoEsperadoParaExecutar; // exemplo : 100 ms
    int pid; // para entrar em contacto com o cliente
    struct timeval start; //tempo de inicio atualizar o valor quando chega ao servidor
    struct timeval end; // quando o filho termina de executar
    char nomePrograma[300]; // argv3 stringSize valor estatico ex 30 "ls cat serv" filho no server vai partir a string por espaços 300 parece pequeno para a flag -p
}*ect,Elemento; // Elemento = struct comicacaoServer

"""""
Lista no servidor vai ser uma lista de elemento

Elemento lista[4048]; //numero 2^n

Filho termina de executar e comunica ao pai  -> pai escreve para no mesmo ficheiro (escrever o que esta no enunciado)
"""""

// Exemplo de outro algoritmo sem ser o FIFO (First in first out - executar por ordem de chegada), ter a lista ordenada por os que demoram menos tempo a executar 

// FIFO  1º 100 ms 2º 3000 ms 3º 5 ms primeiro a chegar primeiro a executar

// Outra maneira ordenar pelo ordem crescente do tempo 1º 5 ms 2º 100 ms 3º 3000 ms tambem pode ser procurar o mais pequeno adicionas a 
// lista no fim e quando for para executar um procurar o que tem menor valor de tempo