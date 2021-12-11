#include <string>
#include <cstring>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <iostream>

#include <stdio.h>

#include "funcoes_servidor.h"
#include "mensagem.h"
#include "list.h"
#include "comms.h"

using std::cout;
using std::cerr;
using std::endl;


void cd_servidor (int soquete, int *seq, struct mensagem *res){
	string diretorio;
    
	for(int i: res->dados)
		diretorio.push_back(i);

    int ret = chdir(diretorio.c_str());
    if(ret == 0){
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
        envia_mensagem(soquete, ack);
    } else {
        string num_erro = "0";
        if(errno == EACCES){
            num_erro = '1';
        } else if(errno == ENOENT || errno == ENOTDIR){
            num_erro = '2';
        }

        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, 0b10, 0b01, *seq);
        envia_mensagem(soquete, erro);
    }

    *seq = (*seq+1)%16;
}

void ls_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *msg;
    string dados, parte;
    int qt_msg, ls_tam, parte_tam;

    dados = ls(".");
    ls_tam = dados.length();

    parte_tam = (ls_tam >= 15? 15 : ls_tam);
    parte = dados.substr(0, parte_tam);

    msg = monta_mensagem("ls_dados", parte,  0b10, 0b01, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
    }while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;

    for(int i = 15; i < ls_tam; i+=15){
        parte_tam = (ls_tam-i >= 15? 15 : ls_tam-i);
        parte = dados.substr(i, parte_tam);
        msg = monta_mensagem("ls_dados", parte,  0b10, 0b01, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;			
    }
    
    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
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
        erro = monta_mensagem("erro", num_erro, 0b10, 0b01, *seq);
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
        parte_tam = (dados_tam >= 15? 15 : dados_tam);
        parte = dados.substr(0, parte_tam);
        
        msg = monta_mensagem("conteudo", parte,  0b10, 0b01, *seq);
        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;

        for(int i = 15; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", parte,  0b10, 0b01, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, 0b01, *seq);
            } while(res->tipo == 0b1001 || *seq != res->seq);
        
            *seq = (*seq + 1) % 16;			
        }

        j++;
    }
    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
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
        erro = monta_mensagem("erro", num_erro, 0b10, 0b01, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        return;
    }

    struct mensagem *ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq + 1) % 16;

    // precisa aguardar o numero de sequencia aumentar no lado do cliente
    do{
        res = espera_mensagem(soquete, 0b01, *seq);
    } while(string_mensagem(res->tipo) != "linha_dados" || res->seq != *seq);

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

    msg = monta_mensagem("conteudo", parte,  0b10, 0b01, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;

    for(int i = 15; i < dados_tam; i+=15){
        parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
        parte = dados.substr(i, parte_tam);
        msg = monta_mensagem("conteudo", parte,  0b10, 0b01, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);
    
        *seq = (*seq + 1) % 16;			
    }

    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
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
        erro = monta_mensagem("erro", num_erro, 0b10, 0b01, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        cout << "erro no linhas" << endl;
        return;
    }

    struct mensagem *ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq + 1) % 16;

    // precisa aguardar o numero de sequencia aumentar no lado do cliente
    do{
        res = espera_mensagem(soquete, 0b01, *seq);
    } while(string_mensagem(res->tipo) != "linha_dados" || res->seq != *seq);

    std::ifstream myfile(arquivo);
    string linha, linhas;

    for(int i: res->dados)
        linhas.push_back(i);

    string linha_inicial = strtok((char *)linhas.c_str(), "-");
    string linha_final = strtok(NULL, "\n");

    int j = 1;
    // avança até a linha necessária
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

        msg = monta_mensagem("conteudo", parte,  0b10, 0b01, *seq);
        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;

        for(int i = 15; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", parte,  0b10, 0b01, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, 0b01, *seq);
            } while(res->tipo == 0b1001 || *seq != res->seq);
        
            *seq = (*seq + 1) % 16;			
        }
        j++;

    }
    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;
}

void edit_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *ack;
    string texto, arquivo, linha, num_erro;
    
    for(int i = 0; i < res->tam ; i++)
        arquivo.push_back(res->dados[i]);

    num_erro = checa_arquivo(arquivo);

    if(!num_erro.empty()){
        struct mensagem *erro;
        erro = monta_mensagem("erro", num_erro, 0b10, 0b01, *seq);
        envia_mensagem(soquete, erro);
        *seq = (*seq+1)%16;
        cout << "erro no edit" << endl;
        return;
    }

    ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;
    cout << *seq << endl;
    cout << "morre aqui antes lol" << endl;
    do{
        res = espera_mensagem(soquete, 0b01, *seq);
        cout << (int) res->seq << ' ' << *seq << endl;
    }while(res->tipo != 0b1010 || *seq != res->seq);

    ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;
    
    for(int i = 0; i < res->tam ; i++)
        linha.push_back(res->dados[i]);

    do{
        res = espera_mensagem(soquete, 0b01, *seq);
        if(res->tipo == 0b1100 && *seq == res->seq){
            ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
            envia_mensagem(soquete, ack);
            *seq = (*seq+1)%16;
            for(int i = 0; i < res->tam ; i++)
                texto.push_back(res->dados[i]);
            
        }
    } while(res->tipo != 0b1101 || *seq != res->seq);
        
    ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
    envia_mensagem(soquete, ack);
    *seq = (*seq+1)%16;

    string sed = "sed -i '" + linha + "s/.*/" + texto + "/' " + arquivo;
    system(sed.c_str());
    // checar se a linha é a n+1, então criar ela
    // editar o texto na linha :)
}

void compilar_servidor(int soquete, int *seq, struct mensagem *res){
    struct mensagem *ack, *msg;
    string arquivo, flags;
    
    for(int i = 0; i < res->tam ; i++)
        arquivo.push_back(res->dados[i]);
    *seq = (*seq+1)%16;

    do{
        ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
        envia_mensagem(soquete, ack);

        res = espera_mensagem(soquete, 0b01, *seq);
        if(*seq == res->seq){
            for(int i = 0; i < res->tam ; i++)
                flags.push_back(res->dados[i]);

            *seq = (*seq+1)%16;
        }
    } while(res->tipo != 0b1101);
        
    ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
    envia_mensagem(soquete, ack);
    
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
        parte_tam = (dados_tam >= 15? 15 : dados_tam);
        parte = dados.substr(0, parte_tam);
        
        msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, *seq);
        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01, *seq);
        } while(res->tipo == 0b1001);

        *seq = (*seq + 1) % 16;

        for(int i = 15; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, 0b01, *seq);
            } while(res->tipo == 0b1001);
        
            *seq = (*seq + 1) % 16;			
        }
    }

    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01, *seq);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;

    if(pclose(fp))  {
        // tratar erro ou mandar pro cliente? mas já acabou a mensagem importante aqui
    }
}