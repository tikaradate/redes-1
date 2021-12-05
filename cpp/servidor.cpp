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
#include <fstream>
#include <string>

#include <dirent.h>
#include <unistd.h>
#include <cmath>

#include "comms.h"
#include "mensagem.h"
#include "list.h"

using std::cout;
using std::endl;
using std::string;


#define ENVIA 1
#define RECEBE -1

// void envia_nack(){

// }

int main(){
	int soquete;
	soquete = raw_socket("lo");

	int seq = 0;
	while(1){
		uint8_t res_pacote[19];
		uint8_t *pacote;
		int bytes;
		int paridade;
		struct mensagem *msg, *res;

		res = espera_mensagem(soquete, 0b01);

		paridade = res->tam ^ res->seq ^ res->tipo;
		for(int i = 0; i < res->tam; i++){
			paridade ^= res->dados[i]; 
		}
		if(paridade != res->paridade){
			continue;
		}
		if(seq != res->seq){
			//cout << "ue? "<<(int) res->seq << endl;
			continue;
		}
		// se ta tudo ok, aumenta o seq	
		char *comando = string_mensagem(res->tipo);
		cout << comando << endl;
		if(strcmp(comando, "cd") == 0){
			string dir;
			for(int i: res->dados)
			 	dir.push_back(i);
			cout << "diretorio:" <<  dir << endl;
			int ret = chdir(dir.c_str());
			if(ret == 0){
				errno = 0;
				struct mensagem *ack;
				char buff[100];
				printf("Diretorio atual: %s\n", getcwd(buff, 100));
				ack = monta_mensagem("ack", NULL, 0b10, 0b01, seq);
				envia_mensagem(soquete, ack);
				seq = (seq+1)%16;
			} else {
				struct mensagem erro;
				cout << "erro ou nack ..." << endl;
				// nack é um loop...
				erro.ini = 0b01111110;
				erro.dst = 0b01;
				erro.src = 0b10;
				erro.tam = 0; // arrumar o erro que veio
				erro.seq = seq;
				erro.tipo = 0b1111;
				erro.paridade = erro.tam ^ erro.seq ^ erro.tipo;
				if(send(soquete, pacote, (4+erro.tam)*4, 0)) 
					perror("Diagnostico");
			}
	
		} else if(strcmp(comando, "ls") == 0){
			string dados;
			int qt_msg, ls_tam, parte_tam;
			dados = ls(".");
			ls_tam = dados.length();

			string parte;
			struct mensagem *res, *msg;

			parte_tam = (ls_tam >= 15? 15 : ls_tam);
			parte = dados.substr(0, parte_tam);

			msg = monta_mensagem("ls_dados", (char *) parte.c_str(),  0b10, 0b01, seq);
			do{
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;

			for(int i = 15; i < ls_tam; i+=15){
				parte_tam = (ls_tam-i >= 15? 15 : ls_tam-i);
				parte = dados.substr(i, parte_tam);
				msg = monta_mensagem("ls_dados", (char *) parte.c_str(),  0b10, 0b01, seq);

				do{
					envia_mensagem(soquete, msg);
					res = espera_mensagem(soquete, 0b01);
				} while(res->tipo == 0b1001);

				seq = (seq + 1) % 16;			
			}
 			
			do{
				msg = monta_mensagem("fim", NULL,  0b10, 0b01, seq);
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;

		} else if(strcmp(comando, "ver") == 0){
			string arquivo;
			for(int i: res->dados)
			 	arquivo.push_back(i);
			std::ifstream myfile(arquivo);
			string linha;
			int j = 1;
			while(getline(myfile, linha)){
				string dados =  std::to_string(j) + '\t' + linha + '\n';
				int parte_tam, dados_tam;
				string parte;

				dados_tam = dados.length();
				parte_tam = (dados_tam >= 15? 15 : dados_tam);
				parte = dados.substr(0, parte_tam);
				
				msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, seq);
				do{
					envia_mensagem(soquete, msg);
					res = espera_mensagem(soquete, 0b01);
				} while(res->tipo == 0b1001);

				seq = (seq + 1) % 16;

				for(int i = 15; i < dados_tam; i+=15){
					parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
					parte = dados.substr(i, parte_tam);
					msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, seq);

					do{
						envia_mensagem(soquete, msg);
						res = espera_mensagem(soquete, 0b01);
					} while(res->tipo == 0b1001);
				
					seq = (seq + 1) % 16;			
				}

				j++;
    		}
			do{
				msg = monta_mensagem("fim", NULL,  0b10, 0b01, seq);
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;

		} else if(strcmp(comando, "linha") == 0){
			string arquivo;
			for(int i: res->dados)
			 	arquivo.push_back(i);
			struct mensagem *ack = monta_mensagem("ack", NULL, 0b10, 0b01, seq);
			
			envia_mensagem(soquete, ack);
			res = espera_mensagem(soquete, 0b01);

			std::ifstream myfile(arquivo);
			string linha;
			int j = 0;
			while(getline(myfile, linha)){
				string dados =  std::to_string(j) + ' ' + linha + '\n';
				int parte_tam, dados_tam;
				string parte;

				dados_tam = dados.length();
				parte_tam = (dados_tam >= 15? 15 : dados_tam);
				parte = dados.substr(0, parte_tam);

				msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, seq);
				do{
					envia_mensagem(soquete, msg);
					res = espera_mensagem(soquete, 0b01);
				} while(res->tipo == 0b1001);

				seq = (seq + 1) % 16;

				for(int i = 15; i < dados_tam; i+=15){
					parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
					parte = dados.substr(i, parte_tam);
					msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, seq);

					do{
						envia_mensagem(soquete, msg);
						res = espera_mensagem(soquete, 0b01);
					} while(res->tipo == 0b1001);
				
					seq = (seq + 1) % 16;			
				}

				j++;
    		}
			do{
				msg = monta_mensagem("fim", NULL,  0b10, 0b01, seq);
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;
		} else if(strcmp(comando, "linhas") == 0){

		} else if(strcmp(comando, "edit") == 0){

		} else if(strcmp(comando, "compilar") == 0){

		}
		
	}
}