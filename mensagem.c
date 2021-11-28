#include <stdio.h>
#include <stdlib.h>
#include "mensagem.h"

int calcula_paridade(struct mensagem msg){
	// paridade vertical <= preciso rever
	// tam, seq, tipo, dados
	// XOR byte a byte <= não entendi essa
}

int* monta_pacote(struct mensagem msg){
	int *pacote = calloc(4+msg.tam+1, sizeof(int));
	pacote[0] = msg.ini;
	pacote[1] = (msg.dst << 6) | (msg.src << 4) | msg.tam;
	pacote[2] = (msg.seq << 4) | msg.tipo;
	for(int i = 0; i < msg.tam; i++){
		pacote[i+3] = msg.dados[i];
	}
	pacote[3+msg.tam] = msg.paridade;

	return pacote;
}

struct mensagem* desmonta_pacote(int *pacote){
	struct mensagem *msg = malloc(sizeof(struct mensagem));
	msg->ini  = pacote[0];
	msg->dst  = pacote[1] & 0b11000000;
	msg->src  = pacote[1] & 0b00110000;
	msg->tam  = pacote[1] & 0b00001111;
	msg->seq  = pacote[2] & 0b11110000;
	msg->tipo = pacote[2] & 0b00001111;
	for(int i = 0; i < msg->tam; i++){
		msg->dados[i] = pacote[i+3]; 
	}
	msg->paridade = pacote[msg->tam + 3];

	return msg;
} 


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