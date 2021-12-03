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
			perror("Erro ao ler");
			exit(1);
		}
		char *comando = strtok(str, " \n");
		struct mensagem *msg;
		if(strcmp(comando, "cd") == 0){
			struct mensagem *res;
			uint8_t res_pacote[19];

			char *arg = strtok(NULL, "\n");
			msg = monta_mensagem(comando, arg, 0b01, 0b10, seq);
			pacote = monta_pacote(*msg);
			do{
				if(send(soquete, pacote, (4+msg->tam)*4, 0)) // tem que multiplicar por 4 por alguma razao que nao descobri ainda
					perror("Diagnostico");
				do{
					int bytes = recv(soquete, res_pacote, 19, 0);
					res = desmonta_pacote(res_pacote);
					printf("esperando resposta cd!\n");
				}while(res->src != 0b10);
				// if(res->tipo == 0b1111){
				//	trata_erro(res);
				// }
			} while(res->tipo == 0b1001); // se for NACK, refaz o processo

			std::cout << "ack recebido\n";
			seq = (seq + 1 > 15? 0 : seq + 1);

		} 
		else if(strcmp(comando, "ls") == 0){
			struct mensagem *res;
			uint8_t res_pacote[19];
			
			msg = monta_mensagem(comando, NULL, 0b01, 0b10, seq);
			pacote = monta_pacote(*msg);
			do{
				if(send(soquete, pacote, (4+msg->tam)*4, 0))
					perror("Diagnostico");
				do{
					int bytes = recv(soquete, res_pacote, 19, 0);
					res = desmonta_pacote(res_pacote);
					// printf("esperando resposta ls!\n");
				}while(res->src != 0b10);
				// cout << "aqui?" << endl;
				// if(res->tipo == 0b1111){
				//	trata_erro(res);
				// }
			} while(res->tipo == 0b1001); // se for NACK, refaz o processo

			std::cout << "ack recebido\n";
			seq = (seq + 1 > 15? 0 : seq + 1);;
			string ls_res;
			for(int i = 0; i < res->tam ; i++)
			 	ls_res.push_back(res->dados[i]);
			while(res->tipo != 0b1101){
				msg = monta_mensagem("ack", NULL, 0b01, 0b10, seq);
				pacote = monta_pacote(*msg);

				send(soquete, pacote, (4+msg->tam)*4, 0);

				do{
					int bytes = recv(soquete, res_pacote, 19, 0);
					res = desmonta_pacote(res_pacote);
					// printf("esperando resposta ls_dados! %d\n", seq);
				} while(res->src != 0b10);
				
				if(seq == res->seq){
					cout << "res->tipo: " <<(int)res->tipo << endl;
					seq = (seq + 1 > 15? 0 : seq + 1);
					for(int i = 0; i < res->tam ; i++)
			 			ls_res.push_back(res->dados[i]);
				}
				
			}
			
			cout << seq << ' ' << (int) res->seq << endl;
			cout << ls_res << endl;
			ls_res.clear();
		}
		// } else if(strcmp(comando, "ver") == 0){
		// 	char *arg = strtok(NULL, "\n");
		// 	msg = monta_mensagem(comando, arg, 0b10, seq);
		// 	fila.push(msg);
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