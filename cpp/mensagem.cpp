#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <inttypes.h> 
#include <string>
#include <cstring>
#include <iostream>

#include "mensagem.h"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

uint8_t* monta_pacote(struct mensagem *msg){
	uint8_t *pacote = (uint8_t *) calloc(4+msg->tam, sizeof(uint8_t));
	pacote[0] = msg->ini;
	pacote[1] = (msg->dst << 6) | (msg->src << 4) | msg->tam;
	pacote[2] = (msg->seq << 4) | msg->tipo;
	for(int i = 0; i < msg->tam; i++){
		pacote[i+3] = msg->dados[i];
	}
	pacote[3+msg->tam] = msg->paridade;

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

void imprime_mensagem(struct mensagem *msg){
	printf("ini: %d\n", msg->ini );
	printf("dst: %d\n", msg->dst );
	printf("src: %d\n", msg->src );
	printf("tam: %d\n", msg->tam );
	printf("seq: %d\n", msg->seq );
	for(int i = 0; i < msg->tam; i++){
		printf("%c", msg->dados[i]);
	}
	printf("\n");
	printf("tipo: %d\n", msg->tipo);
}


int tipo_mensagem(string tipo){
	if(tipo == "cd"){
		return 0b0000;
	} else if (tipo == "ls"){
		return 0b0001;
	} else if (tipo == "ver"){
		return 0b0010;
	} else if (tipo == "linha"){
		return 0b0011;
	} else if (tipo == "linhas"){
		return 0b0100;
	} else if (tipo == "edit"){
		return 0b0101;
	} else if (tipo == "compilar"){
		return 0b0110;
	} else if (tipo == "ack"){
		return 0b1000;
	} else if (tipo == "nack"){
		return 0b1001;
	} else if (tipo == "linha_dados"){
		return 0b1010;
	} else if (tipo == "ls_dados"){
		return 0b1011;
	} else if (tipo == "conteudo"){
		return 0b1100;
	} else if (tipo == "fim"){
		return 0b1101;
	} else if (tipo == "erro"){
		return 0b1111;
	}
} 

string string_mensagem(int tipo){
	switch (tipo)
	{
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
	}
}

struct mensagem *monta_mensagem(string tipo, string dados, int src, int dst, int seq){
	struct mensagem *msg = (struct mensagem *) malloc(sizeof(struct mensagem));
	
	msg->ini = 0b01111110;
	msg->dst = dst;
	msg->src = src;
	msg->tipo = tipo_mensagem(tipo);
	if(dados.empty()) msg->tam = 0;
	else msg->tam = dados.length();
	msg->seq = seq;
	msg->paridade = msg->tam ^ msg->seq ^ msg->tipo;
	for(int i = 0; i < msg->tam; i++){
		msg->dados[i] = dados[i];
		msg->paridade ^= dados[i]; 
	}

	return msg;
}

void envia_mensagem(int soquete, struct mensagem *msg){
	uint8_t *pacote;

	pacote = monta_pacote(msg);
	send(soquete, pacote, (4+msg->tam)*4, 0);
}

struct mensagem *espera_mensagem(int soquete, int src, int seq){
	struct mensagem *res;
	uint8_t res_pacote[19];
	do{
		int bytes = recv(soquete, res_pacote, 19, 0);
		res = desmonta_pacote(res_pacote);
	}while(((int)res->src != src));
	return res;
}

bool checa_paridade(struct mensagem *msg){
	int paridade;
	paridade = msg->tam ^ msg->seq ^ msg->tipo;
	for(int i = 0; i < msg->tam; i++){
		paridade ^= msg->dados[i]; 
	}
	return paridade = msg->paridade;
}

void imprime_erro(struct mensagem *msg){
	switch (atoi((const char*) msg->dados))
	{
	case 1:
		cerr << "Acesso negado" << endl;
		break;
	case 2:
		cerr << "Diretorio inexistente" << endl;
		break;
	case 3:
		cerr << "Arquivo inexistente" << endl;
		break;
	case 4:
		cerr << "Linha inexistente" << endl;
		break;
	default:
		break;
	}
}