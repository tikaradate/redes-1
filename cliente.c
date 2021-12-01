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

#include "mensagem.h"
#include "comms.h"

#define ENVIA 1
#define RECEBE -1

struct mensagem *monta_mensagem(char *comando, char *argumento, int dst, int seq){
	struct mensagem *msg = malloc(sizeof(struct mensagem));
	
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
	struct nodo *primeiro = NULL; // fila simples
	while(1==1){
		if(estado == ENVIA){
			char linha[1024];
			char *str = fgets(linha, 1024, stdin);
			if(!str){
				perror("Erro ao ler");
				exit(1);
    		}

			char *comando = strtok(str, " \n");
			char *arg = NULL;
			struct mensagem *msg;

			if(strcmp(comando, "cd") == 0){
				arg = strtok(NULL, "\n");
				monta_mensagem(comando, arg, 0b10, seq);
				enfileira(&primeiro, msg);
			} else if(strcmp(comando, "ver") == 0){
				arg = strtok(NULL, "\n");
			} else if(strcmp(comando, "linha") == 0){
				arg = strtok(NULL, "\n");
			} else if(strcmp(comando, "linhas") == 0){
				arg = strtok(NULL, "\n");
			} else if(strcmp(comando, "edit") == 0){
				arg = strtok(NULL, "\n");
			} else if(strcmp(comando, "compilar") == 0){
				arg = strtok(NULL, "\n");
			}

			pacote = monta_pacote(*msg);
			imprime_mensagem(*msg);
			if(send(soquete, pacote, (4+msg->tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
				perror("Diagnostico");
			
			// estado = RECEBE;
		} else if (estado == RECEBE){
			// para e espera ta errado:
			// tem que checar o destino e quem enviou
			// pois se não a mensagem recém enviada é pega e lida aqui
			uint8_t pacote[19];
			int bytes;
			int paridade;
			bytes = read(soquete, pacote, 20);
			struct mensagem *res = desmonta_pacote(pacote);
			if(res->dst == 0b10){
				continue;
			}

			imprime_mensagem(*res);
			estado = ENVIA;
		}
	}
}