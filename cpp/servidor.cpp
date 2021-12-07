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

#include "funcoes_servidor.h"
#include "comms.h"
#include "mensagem.h"
#include "list.h"

using std::cout;
using std::endl;
using std::string;

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
			continue;
		}
		// se ta tudo ok, aumenta o seq	
		string comando = string_mensagem(res->tipo);
		// cout << comando << endl;
		if(comando == "cd"){
			string diretorio;
			for(int i: res->dados)
			 	diretorio.push_back(i);

			cd_servidor(soquete, &seq, diretorio);
	
		} else if(comando == "ls"){
			ls_servidor(soquete, &seq);

		} else if(comando == "ver"){
			string arquivo;
			for(int i: res->dados)
			 	arquivo.push_back(i);
			
			ver_servidor(soquete, &seq, arquivo);
		} else if(comando == "linha"){
			string arquivo;

			for(int i: res->dados)
			 	arquivo.push_back(i);

			struct mensagem *ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
			envia_mensagem(soquete, ack);
			seq = (seq + 1) % 16;

			// precisa aguardar o numero de sequencia aumentar no lado do cliente
			do{
				res = espera_mensagem(soquete, 0b01);
			} while(res->seq != seq);

			std::ifstream myfile(arquivo);
			string linha, n_linha;

			for(int i: res->dados)
			 	n_linha.push_back(i);

			int j = 0;
			while(getline(myfile, linha) && j < stoi(n_linha)){
				j++;
			}
			
			string dados = linha + '\n';
			int parte_tam, dados_tam;
			string parte;

			dados_tam = dados.length();
			parte_tam = (dados_tam >= 15? 15 : dados_tam);
			parte = dados.substr(0, parte_tam);

			msg = monta_mensagem("conteudo", parte,  0b10, 0b01, seq);
			do{
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;

			for(int i = 15; i < dados_tam; i+=15){
				parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
				parte = dados.substr(i, parte_tam);
				msg = monta_mensagem("conteudo", parte,  0b10, 0b01, seq);

				do{
					envia_mensagem(soquete, msg);
					res = espera_mensagem(soquete, 0b01);
				} while(res->tipo == 0b1001);
			
				seq = (seq + 1) % 16;			
    		}
			do{
				msg = monta_mensagem("fim", "",  0b10, 0b01, seq);
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;
		} else if(comando == "linhas"){
			string arquivo;
			for(int i: res->dados)
			 	arquivo.push_back(i);

			struct mensagem *ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
			envia_mensagem(soquete, ack);
			seq = (seq + 1) % 16;

			// precisa aguardar o numero de sequencia aumentar no lado do cliente
			do{
				res = espera_mensagem(soquete, 0b01);
			} while(res->seq != seq);

			std::ifstream myfile(arquivo);
			string linha, linhas;

			for(int i: res->dados)
			 	linhas.push_back(i);

			cout << linhas << endl;
			string linha_inicial = strtok((char *)linhas.c_str(), "-");
			string linha_final = strtok(NULL, "\n");

			int j = 1;
			while(getline(myfile, linha) && j < stoi(linha_inicial)){
				j++;
			}

			while(getline(myfile, linha) && j < stoi(linha_final)){
				string dados = linha + '\n';
				int parte_tam, dados_tam;
				string parte;

				dados_tam = dados.length();
				parte_tam = (dados_tam >= 15? 15 : dados_tam);
				parte = dados.substr(0, parte_tam);

				msg = monta_mensagem("conteudo", parte,  0b10, 0b01, seq);
				do{
					envia_mensagem(soquete, msg);
					res = espera_mensagem(soquete, 0b01);
				} while(res->tipo == 0b1001);

				seq = (seq + 1) % 16;

				for(int i = 15; i < dados_tam; i+=15){
					parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
					parte = dados.substr(i, parte_tam);
					msg = monta_mensagem("conteudo", parte,  0b10, 0b01, seq);

					do{
						envia_mensagem(soquete, msg);
						res = espera_mensagem(soquete, 0b01);
					} while(res->tipo == 0b1001);
				
					seq = (seq + 1) % 16;			
				}
				j++;

			}
			do{
				msg = monta_mensagem("fim", "",  0b10, 0b01, seq);
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;
		} else if(comando == "edit"){
			struct mensagem *ack;
			string texto, arquivo, linha;
			
			for(int i = 0; i < res->tam ; i++)
				arquivo.push_back(res->dados[i]);

			ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
			seq = (seq+1)%16;
			envia_mensagem(soquete, ack);

			do{
				res = espera_mensagem(soquete, 0b01);
			}while(res->tipo != 0b1010);

			for(int i = 0; i < res->tam ; i++)
				linha.push_back(res->dados[i]);

			seq = (seq+1)%16;
			do{
				ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
				envia_mensagem(soquete, ack);

				res = espera_mensagem(soquete, 0b01);
				if(seq == res->seq){
					for(int i = 0; i < res->tam ; i++)
						texto.push_back(res->dados[i]);
					seq = (seq+1)%16;
				}
			} while(res->tipo != 0b1101);
				
			ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
			envia_mensagem(soquete, ack);
			
			string sed = "sed -i '" + linha + "s/.*/" + texto + "/' " + arquivo;
			system(sed.c_str());
			// checar se a linha é a n+1, então criar ela
			// editar o texto na linha :)

		} else if(comando == "compilar"){
			struct mensagem *ack;
			string arquivo, flags;
			
			for(int i = 0; i < res->tam ; i++)
				arquivo.push_back(res->dados[i]);
			seq = (seq+1)%16;

			do{
				ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
				envia_mensagem(soquete, ack);

				res = espera_mensagem(soquete, 0b01);
				if(seq == res->seq){
					for(int i = 0; i < res->tam ; i++)
						flags.push_back(res->dados[i]);

					seq = (seq+1)%16;
				}
			} while(res->tipo != 0b1101);
				
			ack = monta_mensagem("ack", "", 0b10, 0b01, seq);
			envia_mensagem(soquete, ack);
			
			// cout << flags << endl;
			string gcc_comando = "gcc " + flags + arquivo + " 2>&1";
			cout << gcc_comando << endl;
			
			// FILE *fp = popen((char *) gcc_comando.c_str(), "r");
			char buf[128];
			string compilar_res;
			FILE *fp;

			if ((fp = popen((char *) gcc_comando.c_str(), "r")) == NULL) {
				printf("Error opening pipe!\n");
				return -1;
			}

			while (fgets(buf, 128, fp) != NULL) {
				string dados =  buf;
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
			}

			do{
				msg = monta_mensagem("fim", "",  0b10, 0b01, seq);
				envia_mensagem(soquete, msg);
				res = espera_mensagem(soquete, 0b01);
			} while(res->tipo == 0b1001);

			seq = (seq + 1) % 16;

			if(pclose(fp))  {
				printf("Command not found or exited with error status\n");
				return -1;
    		}


			cout << compilar_res << endl;
		}
		
	}
}