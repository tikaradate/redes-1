#ifndef __FUNCOES_CLIENTE__
#define __FUNCOES_CLIENTE__
#include <string>
#include <iostream>
#include "funcoes_cliente.h"
#include "mensagem.h"

using std::cout;
using std::cerr;
using std::endl;

void cd_cliente(int soquete, int *seq, string diretorio){
    struct mensagem *msg;
    struct mensagem *res;
    
    msg = monta_mensagem("cd", diretorio, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(string_mensagem(res->tipo) == "nack" || *seq != res->seq);
    
    if(string_mensagem(res->tipo) == "erro"){
        imprime_erro(res);
    }

    *seq = (*seq+1)%16;
}

string ls_cliente(int soquete, int *seq){
    struct mensagem *msg;
    struct mensagem *res;
    struct mensagem *ack;
    string ls_res;
			
    msg = monta_mensagem("ls", "", 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1011 || *seq != res->seq);
    
    msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;

    for(int i = 0; i < res->tam ; i++)
        ls_res.push_back(res->dados[i]);

    do{
        res = espera_mensagem(soquete, 0b10, *seq);
        if(res->tipo == 0b1011 && res->seq == *seq){
            msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
            envia_mensagem(soquete, msg);
            *seq = (*seq + 1) % 16;

            for(int i = 0; i < res->tam ; i++)
                ls_res.push_back(res->dados[i]);

        }
    } while(res->tipo != 0b1101 || *seq != res->seq);
    
    msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;    

    return ls_res;
}   

string ver_cliente(int soquete, int *seq, string arquivo){
    struct mensagem *msg;
    struct mensagem *res;
    string ver_res;

    msg = monta_mensagem("ver", arquivo, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1100);
    
    *seq = (*seq+1)%16;
    
    for(int i = 0; i < res->tam ; i++)
        ver_res.push_back(res->dados[i]);

    do{
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", 0b01, 0b10, *seq);
        envia_mensagem(soquete, msg);

        res = espera_mensagem(soquete, 0b10, *seq);
        if(*seq == res->seq){
            *seq = (*seq+1)%16;
            for(int i = 0; i < res->tam ; i++)
                ver_res.push_back(res->dados[i]);
        }
    } while(res->tipo != 0b1101);
        
    msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
    envia_mensagem(soquete, msg);

    return ver_res;
}

string linha_cliente(int soquete, int *seq, string arquivo, string linha){
    struct mensagem *msg;
    struct mensagem *res;
    string linha_res;

    msg = monta_mensagem("linha", arquivo, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1000);

    *seq = (*seq+1)%16;

    msg = monta_mensagem("linha_dados", linha, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1100);

    *seq = (*seq+1)%16;

    for(int i = 0; i < res->tam ; i++)
        linha_res.push_back(res->dados[i]);

    do{
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", 0b01, 0b10, *seq);
        envia_mensagem(soquete, msg);

        res = espera_mensagem(soquete, 0b10, *seq);
        if(*seq == res->seq){
            *seq = (*seq+1)%16;
            for(int i = 0; i < res->tam ; i++)
                linha_res.push_back(res->dados[i]);
        }
    } while(res->tipo != 0b1101);
        
    msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
    envia_mensagem(soquete, msg);

    return linha_res;
}

string linhas_cliente(int soquete, int *seq, string arquivo, string linha_inicial, string linha_final){
    struct mensagem *msg;
    struct mensagem *res;
    string linhas_res;
    
    msg = monta_mensagem("linhas", (char *)arquivo.c_str(), 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1000);

    *seq = (*seq+1)%16;
    string linhas = linha_inicial + '-' + linha_final;
    msg = monta_mensagem("linha_dados", (char *)linhas.c_str(), 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1100);

    *seq = (*seq+1)%16;

    for(int i = 0; i < res->tam ; i++)
        linhas_res.push_back(res->dados[i]);

    do{
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", 0b01, 0b10, *seq);
        envia_mensagem(soquete, msg);

        res = espera_mensagem(soquete, 0b10, *seq);
        if(*seq == res->seq){
            *seq = (*seq+1)%16;
            for(int i = 0; i < res->tam ; i++)
                linhas_res.push_back(res->dados[i]);
        }
    } while(res->tipo != 0b1101);
        
    msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
    envia_mensagem(soquete, msg);

    return linhas;
}

void edit_cliente(int soquete, int *seq, string arquivo, string linha, string texto){
    struct mensagem *msg;
    struct mensagem *res;
    string edit_res, parte;
    int parte_tam, texto_tam;


    msg = monta_mensagem("edit", arquivo, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;

    msg = monta_mensagem("linha_dados", linha, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;

    // tem que jogar fora as aspas
    texto_tam = texto.length();

    for(int i = 0; i < texto_tam; i+=15){
        parte_tam = (texto_tam-i >= 15? 15 : texto_tam-i);
        parte = texto.substr(i, parte_tam);
        msg = monta_mensagem("conteudo", parte,   0b01, 0b10, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b10, *seq);
        } while(res->tipo == 0b1001);

        *seq = (*seq + 1) % 16;			
    }
    
    do{
        msg = monta_mensagem("fim", "",   0b01, 0b10, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;
}

string compilar_cliente(int soquete, int *seq, string arquivo, string opcoes){
    struct mensagem *msg;
    struct mensagem *res;
    string edit_res, parte, compilar_res;
    int parte_tam, opcoes_tam;

    msg = monta_mensagem("compilar", arquivo, 0b01, 0b10, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo == 0b1001);

    *seq = (*seq + 1) % 16;
    
    opcoes_tam = opcoes.length();
    for(int i = 0; i < opcoes_tam; i+=15){
        parte_tam = (opcoes_tam-i >= 15? 15 : opcoes_tam-i);
        parte = opcoes.substr(i, parte_tam);
        msg = monta_mensagem("conteudo", parte,   0b01, 0b10, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, 0b10, *seq);
        } while(res->tipo == 0b1001);

        *seq = (*seq + 1) % 16;			
    }
    
    do{
        msg = monta_mensagem("fim", "",   0b01, 0b10, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo == 0b1100);

    *seq = (*seq + 1) % 16;

    // aguarda o conteúdo ou erro após enviar tudo do compilar
    do{
        res = espera_mensagem(soquete, 0b10, *seq);
    } while(res->tipo != 0b1100);

    for(int i = 0; i < res->tam ; i++)
        compilar_res.push_back(res->dados[i]);

    do{
        struct mensagem *ack;
        ack = monta_mensagem("ack", "", 0b01, 0b10, *seq);
        envia_mensagem(soquete, msg);

        res = espera_mensagem(soquete, 0b10, *seq);
        if(*seq == res->seq){
            *seq = (*seq+1)%16;
            for(int i = 0; i < res->tam ; i++)
                compilar_res.push_back(res->dados[i]);
        }
    } while(res->tipo != 0b1101);
        
    msg = monta_mensagem("ack", "", 0b01, 0b10, *seq);
    envia_mensagem(soquete, msg);

    return compilar_res;
}

#endif