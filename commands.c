#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mensagem.h"

int main(){
    char line[1024];
    char *str = fgets(line, 1024, stdin);
    if(!str){
        perror("Erro ao ler");
        exit(1);
    }
    // int i = 0;
    // char comando[9];
    // while(str[i] != ' ' && str[i] != '\n'){
    //     comando[i] = str[i];
    //     i++;
    // }
    // comando[i] = '\0';
    // printf("%s\n", comando);
    char *comando = strtok(str, " \n");
    char *diretorio;
    if(strcmp(comando, "cd") == 0){
		diretorio = strtok(NULL, " \n");
        printf("%s\n", diretorio);
	} 
    // else if (strcmp(comando, "ls") == 0){
	// 	return 0b0001;
	// } else if (strcmp(comando, "ver") == 0){
	// 	return 0b0010;
	// } else if (strcmp(comando, "linha") == 0){
	// 	return 0b0011;
	// } else if (strcmp(comando, "linhas") == 0){
	// 	return 0b0100;
	// } else if (strcmp(comando, "edit") == 0){
	// 	return 0b0101;
	// } else if (strcmp(comando, "compilar") == 0){
	// 	return 0b0110;
	// }
    // ver como tratar os argumentos
    // acho que vai ter que ser tudo separado
    // o pior é o compilar que tem n flags ...
}