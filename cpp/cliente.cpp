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

#include <vector>
#include <iostream>

#include "list.h"
#include "mensagem.h"
#include "comms.h"
#include "funcoes_cliente.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;


int main(){
	int soquete;
 	soquete = raw_socket("lo");
	int seq = 0;
	uint8_t *pacote;
	while(2!=1){
		char linha[1024];
		char *str = fgets(linha, 1024, stdin);
		if(!str){
			perror("Erro ao ler comando");
			exit(1);
		}
		char *comando = strtok(str, " \n");
		struct mensagem *msg;
		if(strcmp(comando, "cd") == 0){
			string diretorio = strtok(NULL, "\n");
			cd_cliente(soquete, &seq, diretorio);
		} else if(strcmp(comando, "ls") == 0){
			string ls;
			ls = ls_cliente(soquete, &seq);
			cout << ls << endl;
		} else if(strcmp(comando, "ver") == 0){
			string ver_res;
			string arquivo = strtok(NULL, "\n");
			ver_res = ver_cliente(soquete, &seq, arquivo);

			cout << ver_res << endl;
		} else if(strcmp(comando, "linha") == 0){
			string linha_res;
			string linha = strtok(NULL, " ");
			string arquivo = strtok(NULL, "\n");
			
			linha_res = linha_cliente(soquete, &seq, arquivo, linha);
			cout << linha_res << endl;
		} else if(strcmp(comando, "linhas") == 0){
			string linhas_res;
			string linha_inicial = strtok(NULL, " ");
			string linha_final = strtok(NULL, " ");
			string arquivo = strtok(NULL, "\n");
			
			linhas_res = linhas_cliente(soquete, &seq,arquivo, linha_inicial, linha_final);
			cout << linhas_res << endl;
		} else if(strcmp(comando, "edit") == 0){
			string edit_res;
			string linha = strtok(NULL, " ");
			string arquivo = strtok(NULL, " ");
			string texto = strtok(NULL, "\n");

			edit_cliente(soquete, &seq, arquivo, linha, texto);
		} else if(strcmp(comando, "compilar") == 0){
			char *argumento;
			string compilar_res, arquivo, opcoes;
			vector<string> compilar_args;

			// necessário um tratamento para pegar as opcoes
			argumento = strtok(NULL, " ");
			while(argumento){
				compilar_args.push_back(argumento);
				argumento = strtok(NULL, " ");
			}

			// pega o ultimo argumento, que é o arquivo
			arquivo = compilar_args.back();
			arquivo.pop_back();
			compilar_args.pop_back();
			// vai pegando as opcoes até acabar
			while(!compilar_args.empty()){
				opcoes.append(compilar_args.back() + ' ');
				compilar_args.pop_back();
			}

			compilar_res = compilar_cliente(soquete, &seq, arquivo, opcoes);

			cout << compilar_res << endl;
		} else if(strcmp(comando, "lcd") == 0){
			string diretorio = strtok(NULL, "\n");
			chdir(diretorio.c_str());
		} else if(strcmp(comando, "lls") == 0){
			cout << ls(".");
		}
	}
}