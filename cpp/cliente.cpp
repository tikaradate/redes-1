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
// }

#include <queue>

#include "mensagem.h"
#include "comms.h"

#define ENVIA 1
#define RECEBE -1

struct mensagem *monta_mensagem(char *comando, char *argumento, int dst, int seq){
	struct mensagem *msg = (struct mensagem *) malloc(sizeof(struct mensagem));
	
	msg->ini = 0b01111110;
	msg->dst = dst;
	if(dst == 01) msg->src = 0b10;
	else msg->src = 0b01;
	msg->tipo = tipo_mensagem(comando);
	if(!argumento) msg->tam = 0;
	else msg->tam = strlen(argumento);
	msg->seq = seq;
	msg->paridade = msg->tam ^ msg->seq ^ msg->tipo;
	for(int i = 0; i < msg->tam; i++){
		msg->dados[i] = argumento[i];
		msg->paridade ^= argumento[i]; 
	}

	return msg;
}

int main(){
	int soquete;
 	soquete = raw_socket("lo");
	int estado = ENVIA;
	int seq = 0;
	uint8_t *pacote;
	std::queue<struct mensagem*> fila;
	while(1==1){
		if(estado == ENVIA){
			char linha[1024];
			char *str = fgets(linha, 1024, stdin);
			if(!str){
				perror("Erro ao ler");
				exit(1);
    		}
			
			// se não estiver vazia a fila, checa por mensagens e 
			// retira a mensagem da fila ao receber um ACK
			// se receber um NACK, reenvia a mensagem no topo da fila :)
			if(!fila.empty()){
				uint8_t pacote[19];
				struct mensagem *res;
				int bytes;
				int paridade;
				bytes = recv(soquete, pacote, 19, 0);
				res = desmonta_pacote(pacote);
				if(res->dst == 0b01){
					if(res->tipo == 0b1000){
						fila.pop();
						// se não estiver vazia, manda o proximo cara
					} else if(res->tipo == 0b1001){
						// só reenvia o pacote?
						struct mensagem *msg = fila.front();
						uint8_t *pack = monta_pacote(*msg);
						if(send(soquete, pacote, (4+msg->tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
							perror("Diagnostico");
					} else {
						// é algum de resposta, dai age na fila de acordo
					}
				}
			}

			char *comando = strtok(str, " \n");
			struct mensagem *msg;
			if(strcmp(comando, "cd") == 0){
				char *arg = strtok(NULL, "\n");
				msg = monta_mensagem(comando, arg, 0b10, seq);
				fila.push(msg);
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
			} else {
				msg = monta_mensagem(comando, NULL, 0b10, seq);
				fila.push(msg);
			}
			// pensar como mandar a msg, pegar da fila? acho que sim 
			msg = fila.front();
			pacote = monta_pacote(*msg);
			imprime_mensagem(*msg);
			if(send(soquete, pacote, (4+msg->tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
				perror("Diagnostico");
			
			// estado = RECEBE;
		} else if (estado == RECEBE){
			// para e espera ta errado:
			// tem que checar o destino e quem enviou
			// pois se não a mensagem recém enviada é pega e lida aqui
			// uint8_t pacote[19];
			// int bytes;
			// int paridade;
			// bytes = recv(soquete, pacote, 19, 0);
			// struct mensagem *res = desmonta_pacote(pacote);
			// if(res->dst == 0b10){
			// 	continue;
			// }

			// imprime_mensagem(*res);
			// estado = ENVIA;
		}
	}
}