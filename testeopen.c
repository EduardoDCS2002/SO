#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h>  //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(){
    int fdoutput = open("output-folder/output", O_CREAT, O_WRONLY);
    write(fdoutput, "olá", 3);
    close(fdoutput);
    return 0;
}