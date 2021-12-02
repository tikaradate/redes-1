// extern "C"
// {
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <inttypes.h> 

#include <queue>

#include <iostream>

#include "mensagem.h"
#include "comms.h"

#define ENVIA 1
#define RECEBE -1

int main(){
	int soquete;
 	soquete = raw_socket("lo");
	int estado = ENVIA;
	int seq = 0;
	uint8_t *pacote;
	std::queue<struct mensagem*> fila;
	while(1==1){	
		// se não estiver vazia a fila, checa por mensagens e 
		// retira a mensagem da fila ao receber um ACK
		// se receber um NACK, reenvia a mensagem no topo da fila :)	
		char linha[1024];
		char *str = fgets(linha, 1024, stdin);
		if(!str){
			perror("Erro ao ler");
			exit(1);
		}
		char *comando = strtok(str, " \n");
		struct mensagem *msg;
		if(strcmp(comando, "cd") == 0){
			char *arg = strtok(NULL, "\n");
			msg = monta_mensagem(comando, arg, 0b10, seq);
			//fila.push(msg);
			pacote = monta_pacote(*msg);
			if(send(soquete, pacote, (4+msg->tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
				perror("Diagnostico");
			imprime_mensagem(*msg);
			uint8_t *pacote;
			int bytes = recv(soquete, pacote, 19, 0);
			struct mensagem *res = desmonta_pacote(pacote);
			while(res->src == 0b01){
				int bytes = recv(soquete, pacote, 19, 0);
				res = desmonta_pacote(pacote);
			}
			std::cout << "nack recebido\n";
			// checar se ACK, NACK ou erro
		} else if(strcmp(comando, "ls") == 0){
			msg = monta_mensagem(comando, NULL, 0b10, seq);
			//fila.push(msg);
			if(send(soquete, pacote, (4+msg->tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
				perror("Diagnostico");
			
			int bytes = recv(soquete, pacote, 19, 0);
			struct mensagem *res = desmonta_pacote(pacote);
			while(res->dst != 0b01){
				int bytes = recv(soquete, pacote, 19, 0);
				res = desmonta_pacote(pacote);
			}
			// checar se 1011, NACK ou erro
			// e ler os dados...
		} else if(strcmp(comando, "ver") == 0){
			char *arg = strtok(NULL, "\n");
			msg = monta_mensagem(comando, arg, 0b10, seq);
			fila.push(msg);
		} else if(strcmp(comando, "linha") == 0){
			char *linha = strtok(NULL, " \n");
			char *arquivo = strtok(NULL, " \n");
			msg = monta_mensagem(comando, arquivo, 0b10, seq);
			fila.push(msg);
			msg = monta_mensagem(comando, linha, 0b10, seq);
			fila.push(msg);
		} else if(strcmp(comando, "linhas") == 0){
			char *linha_ini = strtok(NULL, " \n");
			char *linha_fim = strtok(NULL, " \n");
			char *arquivo = strtok(NULL, " \n");
			msg = monta_mensagem(comando, arquivo, 0b10, seq);
			fila.push(msg);
			msg = monta_mensagem(comando, linha, 0b10, seq);
			fila.push(msg);
		} else if(strcmp(comando, "edit") == 0){
			// tem que tratar diferente?
			// tipo a parte do texto la
			char *linha_ini = strtok(NULL, " \n");
			char *linha_fim = strtok(NULL, " \n");
			char *arquivo = strtok(NULL, " \n");
			msg = monta_mensagem(comando, arquivo, 0b10, seq);
			fila.push(msg);
			msg = monta_mensagem(comando, linha, 0b10, seq);
			fila.push(msg);
		} else if(strcmp(comando, "compilar") == 0){
			// tem que tratar diferente
			// ler as opcoes e dps separar o nome do arquivo
			char *opcoes = strtok(NULL, " \n");
			char *arquivo = strtok(NULL, " \n");
			msg = monta_mensagem(comando, arquivo, 0b10, seq);
			fila.push(msg);
			msg = monta_mensagem(comando, linha, 0b10, seq);
			fila.push(msg);
		}
	}
}