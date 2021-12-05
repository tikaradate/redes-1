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
#include <unistd.h>

#include <inttypes.h> 

#include <queue>

#include <iostream>

using std::cout;
using std::endl;
using std::string;


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
			perror("Erro ao ler comando");
			exit(1);
		}
		char *comando = strtok(str, " \n");
		struct mensagem *msg;
		if(strcmp(comando, "cd") == 0){
			struct mensagem *res;

			char *arg = strtok(NULL, "\n");
			msg = monta_mensagem(comando, arg, 0b01, 0b10, seq);
			do{
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b10);
			} while(res->tipo == 0b1001);

			seq = (seq+1)%16;

		} 
		else if(strcmp(comando, "ls") == 0){
			struct mensagem *res;
			
			msg = monta_mensagem(comando, NULL, 0b01, 0b10, seq);
			do{
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b10);
			} while(res->tipo != 0b1011);
			
			seq = (seq+1)%16;
			string ls_res;
			for(int i = 0; i < res->tam ; i++)
			 	ls_res.push_back(res->dados[i]);

			do{
				struct mensagem *ack;
				ack = monta_mensagem("ack", NULL, 0b01, 0b10, seq);
				envia_mensagem(soquete, msg);

				res = espera_mensagem(soquete, 0b10);
				if(seq == res->seq){
					seq = (seq+1)%16;
					for(int i = 0; i < res->tam ; i++)
			 			ls_res.push_back(res->dados[i]);
				}
			} while(res->tipo != 0b1101);
				
			msg = monta_mensagem("ack", NULL, 0b01, 0b10, seq);
			envia_mensagem(soquete, msg);

			cout << ls_res << endl;
		} else if(strcmp(comando, "ver") == 0){
			struct mensagem *res;
			char *arg = strtok(NULL, "\n");

			msg = monta_mensagem(comando, arg, 0b01, 0b10, seq);
			do{
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b10);
			} while(res->tipo != 0b1100);
			
			seq = (seq+1)%16;
			string ls_res;
			for(int i = 0; i < res->tam ; i++)
			 	ls_res.push_back(res->dados[i]);

			do{
				struct mensagem *ack;
				ack = monta_mensagem("ack", NULL, 0b01, 0b10, seq);
				envia_mensagem(soquete, msg);

				res = espera_mensagem(soquete, 0b10);
				if(seq == res->seq){
					seq = (seq+1)%16;
					for(int i = 0; i < res->tam ; i++)
			 			ls_res.push_back(res->dados[i]);
				}
			} while(res->tipo != 0b1101);
				
			msg = monta_mensagem("ack", NULL, 0b01, 0b10, seq);
			envia_mensagem(soquete, msg);

			cout << ls_res << endl;
		}
		// } else if(strcmp(comando, "linha") == 0){
		// 	char *linha = strtok(NULL, " \n");
		// 	char *arquivo = strtok(NULL, " \n");
		// 	msg = monta_mensagem(comando, arquivo, 0b10, seq);
		// 	fila.push(msg);
		// 	msg = monta_mensagem(comando, linha, 0b10, seq);
		// 	fila.push(msg);
		// } else if(strcmp(comando, "linhas") == 0){
		// 	char *linha_ini = strtok(NULL, " \n");
		// 	char *linha_fim = strtok(NULL, " \n");
		// 	char *arquivo = strtok(NULL, " \n");
		// 	msg = monta_mensagem(comando, arquivo, 0b10, seq);
		// 	fila.push(msg);
		// 	msg = monta_mensagem(comando, linha, 0b10, seq);
		// 	fila.push(msg);
		// } else if(strcmp(comando, "edit") == 0){
		// 	// tem que tratar diferente?
		// 	// tipo a parte do texto la
		// 	char *linha_ini = strtok(NULL, " \n");
		// 	char *linha_fim = strtok(NULL, " \n");
		// 	char *arquivo = strtok(NULL, " \n");
		// 	msg = monta_mensagem(comando, arquivo, 0b10, seq);
		// 	fila.push(msg);
		// 	msg = monta_mensagem(comando, linha, 0b10, seq);
		// 	fila.push(msg);
		// } else if(strcmp(comando, "compilar") == 0){
			// tem que tratar diferente
			// ler as opcoes e dps separar o nome do arquivo
		// 	char *opcoes = strtok(NULL, " \n");
		// 	char *arquivo = strtok(NULL, " \n");
		// 	msg = monta_mensagem(comando, arquivo, 0b10, seq);
		// 	fila.push(msg);
		// 	msg = monta_mensagem(comando, linha, 0b10, seq);
		// 	fila.push(msg);
		// }
	}
}