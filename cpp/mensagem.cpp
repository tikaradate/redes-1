#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h> 
#include <string.h>

#include "mensagem.h"


int calcula_paridade(struct mensagem msg){
	
}

uint8_t* monta_pacote(struct mensagem msg){
	uint8_t *pacote = (uint8_t *) calloc(4+msg.tam, sizeof(uint8_t));
	pacote[0] = msg.ini;
	pacote[1] = (msg.dst << 6) | (msg.src << 4) | msg.tam;
	pacote[2] = (msg.seq << 4) | msg.tipo;
	for(int i = 0; i < msg.tam; i++){
		pacote[i+3] = msg.dados[i];
	}
	pacote[3+msg.tam] = msg.paridade;

	return pacote;
}

struct mensagem* desmonta_pacote(uint8_t *pacote){
	struct mensagem *msg = (struct mensagem *) calloc(1,sizeof(struct mensagem));
	msg->ini  = pacote[0];
	msg->dst  = (pacote[1] & 0b11000000) >> 6;
	msg->src  = (pacote[1] & 0b00110000) >> 4;
	msg->tam  = pacote[1] & 0b00001111;
	msg->seq  = (pacote[2] & 0b11110000) >> 4;
	msg->tipo = pacote[2] & 0b00001111;
	for(int i = 0; i < msg->tam; i++){
		msg->dados[i] = pacote[i+3]; 
	}
	msg->paridade = pacote[msg->tam + 3];

	return msg;
} 

void imprime_mensagem(struct mensagem msg){
	printf("%d\n", msg.ini );
	printf("%d\n", msg.dst );
	printf("%d\n", msg.src );
	printf("%d\n", msg.tam );
	printf("%d\n", msg.seq );
	for(int i = 0; i < msg.tam; i++){
		printf("%c", msg.dados[i]);
	}
	printf("\n");
	printf("%d\n", msg.tipo);
}


int tipo_mensagem(char *tipo){
	if(strcmp(tipo, "cd") == 0){
		return 0b0000;
	} else if (strcmp(tipo, "ls") == 0){
		return 0b0001;
	} else if (strcmp(tipo, "ver") == 0){
		return 0b0010;
	} else if (strcmp(tipo, "linha") == 0){
		return 0b0011;
	} else if (strcmp(tipo, "linhas") == 0){
		return 0b0100;
	} else if (strcmp(tipo, "edit") == 0){
		return 0b0101;
	} else if (strcmp(tipo, "compilar") == 0){
		return 0b0110;
	} else if (strcmp(tipo, "ack") == 0){
		return 0b1000;
	} else if (strcmp(tipo, "nack") == 0){
		return 0b1001;
	} else if (strcmp(tipo, "linha_dados") == 0){
		return 0b1010;
	} else if (strcmp(tipo, "ls_dados") == 0){
		return 0b1011;
	} else if (strcmp(tipo, "conteudo") == 0){
		return 0b1100;
	} else if (strcmp(tipo, "fim") == 0){
		return 0b1101;
	} else if (strcmp(tipo, "erro") == 0){
		return 0b1111;
	}
} 

// talvez não estritamente necessário
char *string_mensagem(int b_tipo){
	switch (b_tipo){
	case 0b0000:
		return "cd";
	case 0b0001:
	    return "ls";
	case 0b0010:
	  	return "ver";
	case 0b0011:
	  	return "linha";
	case 0b0100:
	  	return "linhas";
	case 0b0101:
	  	return "edit";
	case 0b0110:
	  	return "compilar";
	case 0b1000:
		return "ack";
	case 0b1001:
	    return "nack";
	case 0b1010:
	  	return "linha_dados";
	case 0b1011:
	  	return "ls_dados";
	case 0b1100:
	  	return "conteudo";
	case 0b1101:
	  	return "fim";
	case 0b1111:
	  	return "erro";
		default:
		break;
	}
} 

struct mensagem *monta_mensagem(char *tipo, char *dados, int src, int dst, int seq){
	struct mensagem *msg = (struct mensagem *) malloc(sizeof(struct mensagem));
	
	msg->ini = 0b01111110;
	msg->dst = dst;
	msg->src = src;
	msg->tipo = tipo_mensagem(tipo);
	if(!dados) msg->tam = 0;
	else msg->tam = strlen(dados);
	msg->seq = seq;
	msg->paridade = msg->tam ^ msg->seq ^ msg->tipo;
	for(int i = 0; i < msg->tam; i++){
		msg->dados[i] = dados[i];
		msg->paridade ^= dados[i]; 
	}

	return msg;
}