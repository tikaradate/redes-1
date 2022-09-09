#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h> 

#include <vector>
#include <iostream>

#include "misc.h"
#include "mensagem.h"
#include "funcoes_cliente.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

int main(){
	int soquete;
 	soquete = raw_socket("lo");
	int seq = 0;
	
	while(true){
		char linha[1024];
		char *str = fgets(linha, 1024, stdin);
		if(!str){
			perror("Erro ao ler comando.");
			exit(1);
		}
		char *comando = strtok(str, " \n");
		if(!comando){
			continue;
		}
		if(strcmp(comando, "cd") == 0){
			char *dir = strtok(NULL, "\n"); 
			if(!dir){
				cerr << "Precisa de um diretório\nUso: cd $DIRETORIO" << endl;
				continue;
			} 
			string diretorio(dir);
			cd_cliente(soquete, &seq, diretorio);
		} else if(strcmp(comando, "ls") == 0){
			string ls;
			ls = ls_cliente(soquete, &seq);
			cout << ls << endl;
		} else if(strcmp(comando, "ver") == 0){
			char *arq = strtok(NULL, "\n");
			if(!arq){
				cerr << "Precisa de um arquivo\nUso: ver $ARQUIVO" << endl;
				continue;
			}
			string ver_res;
			string arquivo(arq);
			ver_res = ver_cliente(soquete, &seq, arquivo);
			cout << ver_res << endl;
		} else if(strcmp(comando, "linha") == 0){
			char *linha, *arquivo;

			linha = strtok(NULL, " ");
			arquivo = strtok(NULL, "\n");
			if(!linha || !arquivo){
				cerr << "Faltou algum argumento\nUso: linha $LINHA $ARQUIVO" << endl;
				continue;
			} 

			string linha_res;
			linha_res = linha_cliente(soquete, &seq, arquivo, linha);
			cout << linha_res << endl;
		} else if(strcmp(comando, "linhas") == 0){
			char *inicial, *final, *arquivo;
			inicial = strtok(NULL, " ");
			final = strtok(NULL, " ");
			arquivo = strtok(NULL, "\n");
			if(!inicial || !final || !arquivo){
				cerr << "Faltou algum argumento\nUso: linhas $LINHA_INICIAL $LINHA_FINAL $ARQUIVO" << endl;
				continue;
			}

			string linhas_res;
			linhas_res = linhas_cliente(soquete, &seq,arquivo, inicial, final);
			cout << linhas_res << endl;
		} else if(strcmp(comando, "edit") == 0){
			char *linha, *arquivo, *texto;
			linha = strtok(NULL, " ");
			arquivo = strtok(NULL, " ");
			texto = strtok(NULL, "\n");

			if(!linha || !arquivo || !texto){
				cerr << "Faltou algum argumento\nUso: edit $NUMERO_LINHA $ARQUIVO \"$TEXTO\"" << endl;
				continue;
			}

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
			arquivo.pop_back(); // tira o \n
			compilar_args.pop_back();
			// vai pegando as opcoes até acabar
			while(!compilar_args.empty()){
				opcoes.append(compilar_args.back() + ' ');
				compilar_args.pop_back();
			}

			compilar_res = compilar_cliente(soquete, &seq, arquivo, opcoes);

			cout << compilar_res << endl;
		/* comandos locais */
		} else if(strcmp(comando, "lcd") == 0){
			int ret;
			char *dir = strtok(NULL, "\n"); 
			if(!dir){
				cerr << "Precisa de um diretório\nUso: lcd DIRNAME" << endl;
				continue;
			}
			string diretorio(dir);
			ret = chdir(diretorio.c_str());

			if(ret != 0){
				// acesso negado
				if(errno == EACCES){
					cerr << "Acesso negado." << endl;
				// diretorio inexistente/não é diretorio
				} else if(errno == ENOENT || errno == ENOTDIR){
					cerr << "Diretorio inexistente." << endl;;
				}
		} else if(strcmp(comando, "lls") == 0){
			cout << ls(".") << endl;
		}
	}
}