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
    int tipo = tipo_mensagem(comando);
    if(tipo == 0 || tipo == 1 || tipo == 4)
        char *arg1 = strok(NULL, " \n");
    // ver como tratar os argumentos
    // acho que vai ter que ser tudo separado
    // o pior é o compilar que tem n flags ...
}