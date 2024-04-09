#include <sys/types.h>
#include <unistd.h>  //chamadas ao sistema: defs e decls essenciais
#include <fcntl.h>  //O_RDONLY, O_WRONLY, O_CREAT, O_*
#include <stdio.h>


//forma de correr o client
// ./client execute 100-u "prog-a arg-1 (...) arg-n" output: task n received\n
//ou
// ./client execute 3000-p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n" output: task n received\n
//ou
// ./client status output: Executing...\n Scheduled...\n Completed...\n

int main(int argc, char **argv){   //argc: numero de argumentos presentes no argv; //argv: argumentos passados na inicializaçao do programa (ex: ./client execute -u "...") 
    if(argc<2){
        perror("Need arguments!");
        return -1;
    }
    char *exec_args[20];
	char *string, *cmd, *tofree;
	int i=0;
	tofree = cmd = strdup(**argv);
	while((string = strsep(&cmd," "))!=NULL){
	   exec_args[i]=string;
	   i++;
	}
	exec_args[i]=NULL;
    free(tofree);                                      
    if (argc < 5){                        //5: porque o argv[0] = ./client
        if (argc == 2 &&  strcmp(argv[1],"status") == 0){         //1º evitar segmentation fault, 2º exemplo do enunciado ($ ./client status)
            //TODO: o stor tem que dar a aula
        }else{
            char arr[] = "ERRO! NUMERO DE ARGUMENTOS INAVLIDOS!\n";
            write(STDERR_FILENO,arr,sizeof(arr));
        }
    }else{
        char *comando = argv[1];
        char *time = argv[2];
        char *flag = argv[3];
        char *NomeArgumentos = argv[4];  //TODO: split !no servidor!(perguntar ao stor)
    }
    
    return 0;
}