#ifndef __FUNCOES_SERVIDOR__
#define __FUNCOES_SERVIDOR__

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <string>
#include <fstream>
#include <iostream>

#include "funcoes_servidor.h"
#include "mensagem.h"
#include "misc.h"

using std::cout;
using std::cerr;
using std::endl;

#define CLIENTE 0b01
#define SERVIDOR 0b10

void cd_servidor (int soquete, int *seq, struct mensagem *res){
    string diretorio;
	
    for(int i: res->dados)
		diretorio.push_back(i);

    int ret = chdir(diretorio.c_str());
    if(ret == 0){
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, ack);
    } else {
        string num_erro = "0";
        // acesso negado
        if(errno == EACCES){
            num_erro = '1';
        // diretorio inexistente/não é diretorio
        } else if(errno == ENOENT || errno == ENOTDIR){
            num_erro = '2';
        }

        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, erro);
    }

    *seq = (*seq+1)%16;
}

void ls_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *msg;
    string dados, parte;
    int ls_tam, parte_tam;

    dados = ls(".");
    ls_tam = dados.length();

    for(int i = 0; i < ls_tam; i+=15){
        parte_tam = (ls_tam-i >= 15? 15 : ls_tam-i);
        parte = dados.substr(i, parte_tam);
        msg = monta_mensagem("ls_dados", parte,  0b10, CLIENTE, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, CLIENTE, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;			
    }
    
    do{
        msg = monta_mensagem("fim", "",  0b10, CLIENTE, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;	

}

void ver_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *msg;
    string linha, arquivo, num_erro;

	for(int i: res->dados)
		arquivo.push_back(i);
    
    num_erro = checa_arquivo(arquivo);

    if(!num_erro.empty()){
        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        return;
    }
    
    std::ifstream arquivo_stream(arquivo);

    int j = 1;
    while(getline(arquivo_stream, linha)){
        string dados =  std::to_string(j) + '\t' + linha + '\n';
        int parte_tam, dados_tam;
        string parte;

        dados_tam = dados.length();

        for(int i = 0; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", parte,  0b10, CLIENTE, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, CLIENTE, *seq);
            } while(res->tipo == 0b1001 || *seq != res->seq);
        
            *seq = (*seq + 1) % 16;			
        }

        j++;
    }
    do{
        msg = monta_mensagem("fim", "",  0b10, CLIENTE, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;
}

void linha_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *msg;
    string arquivo, num_erro;

    for(int i: res->dados)
        arquivo.push_back(i);

    num_erro = checa_arquivo(arquivo);

    if(!num_erro.empty()){
        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        return;
    }

    struct mensagem *ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq + 1) % 16;

    // precisa aguardar o numero de sequencia aumentar no lado do cliente
    do{
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(string_mensagem(res->tipo) != "linha_dados" || res->seq != *seq);

    std::ifstream arquivo_stream(arquivo);
    string linha;
    int linhas_arquivo = conta_linhas(arquivo);
    int n_linha;
    
    n_linha = (res->dados[0] << 24) |
              (res->dados[1] << 16) |
              (res->dados[2] << 8 ) |
               res->dados[3]        ;

    cout << n_linha << endl;
    if(n_linha > 0 && n_linha <= linhas_arquivo){
        int j = 1;
        // avança até achar a linha desejada
        if(n_linha > 1){
            while(getline(arquivo_stream, linha) && j < n_linha){
                j++;
            }
        }

        string dados = linha + '\n';
        int parte_tam, dados_tam;
        string parte;

        dados_tam = dados.length();

        for(int i = 0; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", parte,  SERVIDOR, CLIENTE, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, CLIENTE, *seq);
            } while(res->tipo == 0b1001 || *seq != res->seq);
        
            *seq = (*seq + 1) % 16;			
        }
    } else {
        // manda vazio pois a linha não existe
        msg = monta_mensagem("conteudo", "",  SERVIDOR, CLIENTE, *seq);
        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, CLIENTE, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;
    }
    do{
        msg = monta_mensagem("fim", "",  SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;
}

void linhas_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *msg;
    string arquivo, num_erro;

    for(int i: res->dados)
        arquivo.push_back(i);

    num_erro = checa_arquivo(arquivo);

    if(!num_erro.empty()){
        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        cout << "erro no linhas" << endl;
        return;
    }

    struct mensagem *ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq + 1) % 16;

    // precisa aguardar o numero de sequencia aumentar no lado do cliente
    do{
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(string_mensagem(res->tipo) != "linha_dados" || res->seq != *seq);

    std::ifstream arquivo_stream(arquivo);
    string linha;

    int linhas_arquivo = conta_linhas(arquivo);

    int linha_inicial = (res->dados[0] << 24) |
                        (res->dados[1] << 16) |
                        (res->dados[2] << 8 ) |
                         res->dados[3]        ;

    int linha_final   = (res->dados[4] << 24) |
                        (res->dados[5] << 16) |
                        (res->dados[6] << 8 ) |
                         res->dados[7]        ;
    
    

    // avança até a linha necessária
    if(linha_inicial <= linhas_arquivo && linha_inicial > 0 && linha_inicial <= linha_final){
        int j = 1;
        while(getline(arquivo_stream, linha) && j <= linha_final){
            if(j >= linha_inicial){
                string dados = linha + '\n';
                int parte_tam, dados_tam;
                string parte;

                dados_tam = dados.length();
            
                for(int i = 0; i < dados_tam; i+=15){
                    parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
                    parte = dados.substr(i, parte_tam);
                    msg = monta_mensagem("conteudo", parte,  0b10, CLIENTE, *seq);

                    do{
                        envia_mensagem(soquete, msg);
                        res = espera_mensagem(soquete, CLIENTE, *seq);
                    } while(res->tipo == 0b1001 || *seq != res->seq);
                
                    *seq = (*seq + 1) % 16;			
                }
            }
            j++;

        }
    } else {
        msg = monta_mensagem("conteudo", "",  0b10, CLIENTE, *seq);
        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, CLIENTE, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;
    }
    do{
        msg = monta_mensagem("fim", "",  0b10, CLIENTE, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;
}

void edit_servidor(int soquete, int *seq, struct mensagem *res){
    int linha;
    struct mensagem *ack;
    string texto, arquivo, num_erro;
    
    for(int i = 0; i < res->tam ; i++)
        arquivo.push_back(res->dados[i]);

    num_erro = checa_arquivo(arquivo);

    if(!num_erro.empty()){
        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        cout << "erro no edit" << endl;
        return;
    }

    // esta tudo ok
    ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;

    // espera o conteudo de linha do cliente
    do{
        res = espera_mensagem(soquete, CLIENTE, *seq);
    }while(res->tipo != 0b1010 || *seq != res->seq);

    ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;
    
    linha = (res->dados[0] << 24) |
            (res->dados[1] << 16) |
            (res->dados[2] << 8 ) |
             res->dados[3]        ;

    do{
        res = espera_mensagem(soquete, CLIENTE, *seq);
        if(res->tipo == 0b1100 && *seq == res->seq){
            ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
            envia_mensagem(soquete, ack);
            *seq = (*seq+1)%16;
            for(int i = 0; i < res->tam ; i++)
                texto.push_back(res->dados[i]);
            
        }
    } while(res->tipo != 0b1101 || *seq != res->seq);
        
    ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;

    int linhas_arquivo = conta_linhas(arquivo);

    if(linha <= linhas_arquivo){
        // monta o comando sed apropriado
        string sed = "sed -i '" + std::to_string(linha) + "s`.*`" + texto + "`' " + arquivo;
        system(sed.c_str());
    } else if (linha == linhas_arquivo + 1){
        std::ofstream outfile;
        outfile.open(arquivo, std::ios_base::app);
        outfile << '\n' + texto;
    }
        
}

void compilar_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *ack, *msg;
    string arquivo, flags, num_erro;
    
    for(int i = 0; i < res->tam ; i++)
        arquivo.push_back(res->dados[i]);

    num_erro = checa_arquivo(arquivo);

    if(!num_erro.empty()){
        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, SERVIDOR, CLIENTE, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        cout << "erro no compilar" << endl;
        return;
    }

    ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;

    do{
        res = espera_mensagem(soquete, CLIENTE, *seq);
        if(res->tipo == 0b1100 && *seq == res->seq){
            ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
            envia_mensagem(soquete, ack);
            *seq = (*seq+1)%16;

            for(int i = 0; i < res->tam ; i++)
                flags.push_back(res->dados[i]);
        }
    } while(res->tipo != 0b1101 || *seq != res->seq);
    
    ack = monta_mensagem("ack", "", SERVIDOR, CLIENTE, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq + 1) % 16;

    string gcc_comando = "gcc " + flags + arquivo + " 2>&1";

    char buf[128];
    string compilar_res;
    FILE *fp;

    if ((fp = popen((char *) gcc_comando.c_str(), "r")) == NULL) {
        // tratar erro
        // mandar pro cliente?
    }

    while (fgets(buf, 128, fp) != NULL) {
        string dados =  buf;
        int parte_tam, dados_tam;
        string parte;

        dados_tam = dados.length();
        for(int i = 0; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", parte,  0b10, CLIENTE, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, CLIENTE, *seq);
            } while(res->tipo != 0b1000 || *seq != res->seq);
        
            *seq = (*seq + 1) % 16;			
        }
    }

    do{
        msg = monta_mensagem("fim", "",  0b10, CLIENTE, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, CLIENTE, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);
     

    *seq = (*seq + 1) % 16;

    if(pclose(fp))  {
        // tratar erro ou mandar pro cliente? mas já acabou a mensagem importante aqui
    }
}

#endif