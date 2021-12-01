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

#include "comms.h"
#include "mensagem.h"

#define ENVIA 1
#define RECEBE -1

// void envia_nack(){

// }

int main(){
	int soquete;
	soquete = raw_socket("lo");
	
	
	int estado = RECEBE;
	int seq = 0;
	while(1==1){
		if(estado == ENVIA){
			
			estado = RECEBE;
		} else if (estado == RECEBE){
			uint8_t pacote[19];
			int bytes;
			int paridade;
			// todos equivalentes
			// bytes = recvfrom(soquete, c, sizeof(c), 0, (struct sockaddr*) &endereco, &fromlen);
			bytes = recv(soquete, pacote, 19, 0);
			//printf("recv()'d %d bytes of data in buf\n", bytes);
			struct mensagem *msg = desmonta_pacote(pacote);
			if(msg->seq != seq ){
				continue;
			}
			if(msg->dst == 0b01){
				continue;
			}

			paridade = msg->tam ^ msg->seq ^ msg->tipo;
			for(int i = 0; i < msg->tam; i++){
				msg->paridade ^= msg->dados[i]; 
			}

			if(paridade != msg->paridade){
				// envia_nack();
				struct mensagem nack;
				nack.ini = 0b01111110;
				nack.dst = 0b01;
				nack.src = 0b10;
				nack.tam = 0;
				nack.seq = seq;
				nack.tipo = 0b1001;
				nack.paridade = nack.tam ^ nack.seq ^ nack.tipo;
				if(send(soquete, pacote, (4+nack.tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
					perror("Diagnostico ");
				continue;
			}

			seq = (seq+1)%16;
			imprime_mensagem(*msg);
			estado = ENVIA;
		}
	}
}