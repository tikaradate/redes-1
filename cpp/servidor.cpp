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
#include <iostream>

#include <dirent.h>
#include <unistd.h>

#include "comms.h"
#include "mensagem.h"

using std::cout;
using std::endl;

#define ENVIA 1
#define RECEBE -1

// void envia_nack(){

// }

int main(){
	int soquete;
	soquete = raw_socket("lo");
	
	
	int estado = RECEBE;
	int seq = 0;
	while(1){
		uint8_t pacote[19] = {};
		int bytes;
		int paridade;
		// todos equivalentes
		// bytes = recvfrom(soquete, c, sizeof(c), 0, (struct sockaddr*) &endereco, &fromlen);
		bytes = recv(soquete, pacote, 19, 0);
		struct mensagem *msg = desmonta_pacote(pacote);
		while(msg->src == 0b10){
			cout << "socorro" << endl;
			bytes = recv(soquete, pacote, 19, 0);
			msg = desmonta_pacote(pacote);
		}
		paridade = msg->tam ^ msg->seq ^ msg->tipo;
		for(int i = 0; i < msg->tam; i++){
			paridade ^= msg->dados[i]; 
		}
		if(paridade != msg->paridade){
			std::cout << "fudeu irmao\n";
		}
		if(seq != msg->seq){
			continue;
		}
		// se ta tudo ok, aumenta o seq	
		seq = (seq+1)%16;
		char *comando = string_mensagem(msg->tipo);
		cout << comando << endl;
		if(strcmp(comando, "cd") == 0){
			std::string dir;
			for(int i: msg->dados)
			 	dir.push_back(i);
			cout << "diretorio:" <<  dir << endl;
			
			struct mensagem ack;
			ack.ini = 0b01111110;
			ack.dst = 0b01;
			ack.src = 0b10;
			ack.tam = 0;
			ack.seq = seq;
			ack.tipo = 0b1000;
			ack.paridade = ack.tam ^ ack.seq ^ ack.tipo;
			if(send(soquete, pacote, (4+ack.tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
				perror("Diagnostico");
			
			
		} else if(strcmp(comando, "ls") == 0){

		} else if(strcmp(comando, "ver") == 0){

		} else if(strcmp(comando, "linha") == 0){

		} else if(strcmp(comando, "linhas") == 0){

		} else if(strcmp(comando, "edit") == 0){

		} else if(strcmp(comando, "compilar") == 0){

		}
		
	}
}