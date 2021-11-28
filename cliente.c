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

int main(){
	int soquete;
	
	//char b[100] = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n";
	struct mensagem msg;
	msg.ini = 0b01111110;
	msg.dst = 0b11;
	msg.src = 0b11;
	msg.tam = 0;
	msg.seq = 0b0001;
	msg.tipo = 0b0001;
	msg.paridade = 0b00001111;
	// int b[4+msg.tam+1];
	// b[0] = msg.ini;
	// b[1] = (msg.dst << 6) | (msg.src << 4) | msg.tam;
	// b[2] = (msg.seq << 4) | msg.tipo;
	// for(int i = 0; i < msg.tam; i++){
	// 	b[i+3] = msg.dados[i];
	// }
	// b[3+msg.tam] = msg.paridade;
	// //b[3+msg.tam+1] = '\0';

 	// soquete = raw_socket("lo");
	// printf("%d\n",sizeof(b));
	// if(send(soquete, b, 13, 0))
	// 	perror("AAAAAA");
	uint8_t *pacote = monta_pacote(msg);
 	soquete = raw_socket("lo");
	printf("aaaa\n");
	printf("%d\n", pacote[0]);
	printf("%d\n", pacote[1]);
	printf("%d\n", pacote[2]);
	imprime_mensagem(msg);
	if(send(soquete, pacote, (4+msg.tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
		perror("AAAAAA");
	// if(sendto(soquete, b, sizeof(b), 0, (struct sockaddr*) &endereco, sizeof(struct sockaddr_ll)))
	// 	perror("AAAAAA");
}