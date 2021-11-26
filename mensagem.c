#include <stdio.h>
#include <stdlib.h>
#include "mensagem.h"

int calcula_paridade(struct mensagem msg){
	// paridade vertical <= preciso rever
	// tam, seq, tipo, dados
	// XOR byte a byte <= não entendi essa
}

// struct mensagem monta_mensagem(int dst, int src,){
// 	struct mensagem msg;
// 	msg.ini = 0b01111110;
// 	msg.dst = 
// 	msg.src
// 	msg.tam
// 	msg.seq
// 	msg.tipo
// 	msg.dados
// 	msg.paridade
// }

int tipo_mensagem(char *tipo){
	if(strcmp(tipo, "cd")){
		return 0b0000;
	} else if (strcmp(tipo, "ls")){
		return 0b0001;
	} else if (strcmp(tipo, "ver")){
		return 0b0010;
	} else if (strcmp(tipo, "linha")){
		return 0b0011;
	} else if (strcmp(tipo, "linhas")){
		return 0b0100;
	} else if (strcmp(tipo, "edit")){
		return 0b0101;
	} else if (strcmp(tipo, "compilar")){
		return 0b0110;
	}
	fprintf(stderr, "comando não suportado!\n");
	exit(1);
} 