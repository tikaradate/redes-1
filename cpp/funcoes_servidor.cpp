#include <string>
#include <unistd.h>
#include <dirent.h>
#include <fstream>

#include "funcoes_servidor.h"
#include "mensagem.h"
#include "list.h"

void cd_servidor (int soquete, int *seq, string diretorio){
    int ret = chdir(diretorio.c_str());
    if(ret == 0){
        errno = 0;
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", 0b10, 0b01, *seq);
        envia_mensagem(soquete, ack);
        *seq = (*seq+1)%16;
    } else {
        // struct mensagem erro;
        // cout << "erro ou nack ..." << endl;
        // // nack é um loop...
        // erro.ini = 0b01111110;
        // erro.dst = 0b01;
        // erro.src = 0b10;
        // erro.tam = 0; // arrumar o erro que veio
        // erro.seq = *seq;
        // erro.tipo = 0b1111;
        // erro.paridade = erro.tam ^ erro.seq ^ erro.tipo;
        // if(send(soquete, pacote, (4+erro.tam)*4, 0)) 
        // 	perror("Diagnostico");
    }
}

void ls_servidor(int soquete, int *seq){
    struct mensagem *msg;
    struct mensagem *res;
    string dados, parte;
    int qt_msg, ls_tam, parte_tam;

    dados = ls(".");
    ls_tam = dados.length();

    parte_tam = (ls_tam >= 15? 15 : ls_tam);
    parte = dados.substr(0, parte_tam);

    msg = monta_mensagem("ls_dados", (char *) parte.c_str(),  0b10, 0b01, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;

    for(int i = 15; i < ls_tam; i+=15){
        parte_tam = (ls_tam-i >= 15? 15 : ls_tam-i);
        parte = dados.substr(i, parte_tam);
        msg = monta_mensagem("ls_dados", (char *) parte.c_str(),  0b10, 0b01, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01);
        } while(res->tipo == 0b1001);

        *seq = (*seq + 1) % 16;			
    }
    
    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;
}

void ver_servidor(int soquete, int *seq, string arquivo){
    struct mensagem *msg;
    struct mensagem *res;
    string linha;
    std::ifstream myfile(arquivo);
    
    int j = 1;
    while(getline(myfile, linha)){
        string dados =  std::to_string(j) + '\t' + linha + '\n';
        int parte_tam, dados_tam;
        string parte;

        dados_tam = dados.length();
        parte_tam = (dados_tam >= 15? 15 : dados_tam);
        parte = dados.substr(0, parte_tam);
        
        msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, *seq);
        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b01);
        } while(res->tipo == 0b1001);

        *seq = (*seq + 1) % 16;

        for(int i = 15; i < dados_tam; i+=15){
            parte_tam = (dados_tam-i >= 15? 15 : dados_tam-i);
            parte = dados.substr(i, parte_tam);
            msg = monta_mensagem("conteudo", (char *) parte.c_str(),  0b10, 0b01, *seq);

            do{
                envia_mensagem(soquete, msg);
                res = espera_mensagem(soquete, 0b01);
            } while(res->tipo == 0b1001);
        
            *seq = (*seq + 1) % 16;			
        }

        j++;
    }
    do{
        msg = monta_mensagem("fim", "",  0b10, 0b01, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b01);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;
}