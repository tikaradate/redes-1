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

int main(){
	int soquete;
	//char b[100] = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n";
	// struct mensagem msg;
	// msg.ini = 0b01111110;
	// msg.dst = 0b11;
	// msg.src = 0b11;
	// msg.tam = 0;
	// msg.seq = 0b0001;
	// msg.tipo = 0b0001;
	// msg.paridade = 0b00001111;
	// int b[4+msg.tam+1];
	// b[0] = msg.ini;
	// b[1] = (msg.dst << 6) | (msg.src << 4) | msg.tam;
	// b[2] = (msg.seq << 4) | msg.tipo;
	// for(int i = 0; i < msg.tam; i++){
	// 	b[i+3] = msg.dados[i];
	// }
	// b[3+msg.tam] = msg.paridade;
	// //b[3+msg.tam+1] = '\0';

 	soquete = raw_socket("lo");
	// printf("%d\n",sizeof(b));
	// if(send(soquete, b, 13, 0))
	// 	perror("AAAAAA");
	// uint8_t *pacote = monta_pacote(msg);
 	// soquete = raw_socket("lo");
	// printf("aaaa\n");
	// printf("%d\n", pacote[0]);
	// printf("%d\n", pacote[1]);
	// printf("%d\n", pacote[2]);
	// imprime_mensagem(msg);
	// if(send(soquete, pacote, (4+msg.tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
	// 	perror("AAAAAA");
	// if(sendto(soquete, b, sizeof(b), 0, (struct sockaddr*) &endereco, sizeof(struct sockaddr_ll)))
	// 	perror("AAAAAA");

	int estado = ENVIA;
	int seq = 0;
	while(1==1){
		if(estado == ENVIA){
			char linha[1024];
			char *str = fgets(linha, 1024, stdin);
			if(!str){
				perror("Erro ao ler");
				exit(1);
    		}

			char *comando = strtok(str, " \n");
			char *arg1;
			char *arg2;
			char *arg3;
			struct mensagem msg;
			if(strcmp(comando, "cd") == 0){
				arg1 = strtok(NULL, " \n");
			} else if(strcmp(comando, "ver") == 0){
				arg1 = strtok(NULL, " \n");
			} else if(strcmp(comando, "linha") == 0){
				arg1 = strtok(NULL, " \n");
				arg2 = strtok(NULL, " \n");
			} else if(strcmp(comando, "linhas") == 0){
				arg1 = strtok(NULL, " \n");
				arg2 = strtok(NULL, " \n");
			} else if(strcmp(comando, "edit") == 0){
				arg1 = strtok(NULL, " \n");
				arg2 = strtok(NULL, " \n");
				arg3 = strtok(NULL, "\n");
			} else if(strcmp(comando, "compilar") == 0){
				// pode ter 30 argumento bicho
				// como trata isso???
				arg1 = strtok(NULL, " \n");
				arg2 = strtok(NULL, " \n");
			}

			
			msg.ini = 0b01111110;
			
			msg.dst = 0b10;
			msg.src = 0b01;
			msg.tam = strlen(arg1);
			msg.seq = seq++;
			msg.tipo = 0b0000;
			msg.paridade = msg.tam ^ msg.seq ^ msg.tipo;
			for(int i = 0; i < msg.tam; i++){
				msg.dados[i] = arg1[i];
				msg.paridade ^= msg.dados[i]; 
			}

			uint8_t *pacote = monta_pacote(msg);
			imprime_mensagem(msg);
			if(send(soquete, pacote, (4+msg.tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
				perror("Diagnostico ");
			estado = RECEBE;
		} else if (estado == RECEBE){
			// para e espera ta errado
			uint8_t pacote[19];
			int bytes;
			int paridade;
			// todos equivalentes
			// bytes = recvfrom(soquete, c, sizeof(c), 0, (struct sockaddr*) &endereco, &fromlen);
			// bytes = recv(soquete, c, sizeof(c), 0);
			bytes = read(soquete, pacote, 20);
			//printf("recv()'d %d bytes of data in buf\n", bytes);
			struct mensagem *res = desmonta_pacote(pacote);
			// if(msg->seq != seq){
			// 	continue;
			// }
			imprime_mensagem(*res);
			estado = ENVIA;
		}
	}
}