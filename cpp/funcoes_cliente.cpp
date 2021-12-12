#ifndef __FUNCOES_CLIENTE__
#define __FUNCOES_CLIENTE__

#include <string>
#include <iostream>

#include "funcoes_cliente.h"
#include "mensagem.h"

using std::cout;
using std::cerr;
using std::endl;

#define CLIENTE 0b01
#define SERVIDOR 0b10

void cd_cliente(int soquete, int *seq, string diretorio){
    struct mensagem *msg;
    struct mensagem *res;
    
    msg = monta_mensagem("cd", diretorio, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) == "nack" || *seq != res->seq);
    
    if(string_mensagem(res->tipo) == "erro"){
        imprime_erro(res);
    }

    *seq = (*seq+1)%16;
}

string ls_cliente(int soquete, int *seq){
    struct mensagem *msg;
    struct mensagem *res;
    string ls_res;
			
    msg = monta_mensagem("ls", "", CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) == "nack" ||
            (string_mensagem(res->tipo) != "erro" &&
             string_mensagem(res->tipo) != "ls_dados") || 
            *seq != res->seq);

    do{
         if(string_mensagem(res->tipo) == "ls_dados" && res->seq == *seq){
            while(!checa_paridade(res)){
                struct mensagem *nack;
                nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
                envia_mensagem(soquete, nack);
                do{
                    res = espera_mensagem(soquete, SERVIDOR, *seq);
                }while(string_mensagem(res->tipo) != "ls_dados" || *seq != res->seq);
            }

            msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
            envia_mensagem(soquete, msg);
            *seq = (*seq + 1) % 16;

            for(int i = 0; i < res->tam ; i++)
                ls_res.push_back(res->dados[i]);
        }
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    
    while(!checa_paridade(res)){
        struct mensagem *nack;
        nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
        envia_mensagem(soquete, nack);
        do{
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        }while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    }

    msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;    

    return ls_res;
}   

string ver_cliente(int soquete, int *seq, string arquivo){
    struct mensagem *msg;
    struct mensagem *res;
    string ver_res;

    msg = monta_mensagem("ver", arquivo, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) == "nack" ||
            (string_mensagem(res->tipo) != "erro" &&
             string_mensagem(res->tipo) != "conteudo") || 
            *seq != res->seq);
    
    if(string_mensagem(res->tipo) == "erro"){
        imprime_erro(res);
        *seq = (*seq+1)%16;
        return "";
    }

    while(!checa_paridade(res)){
        struct mensagem *nack;
        nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
        envia_mensagem(soquete, nack);
        do{
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        }while(string_mensagem(res->tipo) != "conteudo" || *seq != res->seq);
    }

    msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;
    
    for(int i = 0; i < res->tam ; i++)
        ver_res.push_back(res->dados[i]);

    do{
        if(string_mensagem(res->tipo) == "conteudo"  && res->seq == *seq){
            while(!checa_paridade(res)){
                struct mensagem *nack;
                nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
                envia_mensagem(soquete, nack);
                do{
                    res = espera_mensagem(soquete, SERVIDOR, *seq);
                }while(string_mensagem(res->tipo) != "conteudo" || *seq != res->seq);
            }

            msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
            envia_mensagem(soquete, msg);
            *seq = (*seq + 1) % 16;
            
            for(int i = 0; i < res->tam ; i++)
                ver_res.push_back(res->dados[i]);
        }

        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) != "fim"|| *seq != res->seq);

    while(!checa_paridade(res)){
        struct mensagem *nack;
        nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
        envia_mensagem(soquete, nack);
        do{
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        }while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    }

    msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;

    return ver_res;
}

string linha_cliente(int soquete, int *seq, string arquivo, string linha){
    struct mensagem *msg;
    struct mensagem *res;
    string linha_res;

    msg = monta_mensagem("linha", arquivo, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) == "nack" ||
            (string_mensagem(res->tipo) != "erro" &&
             string_mensagem(res->tipo) != "ack") || 
            *seq != res->seq);

    if(string_mensagem(res->tipo) == "erro"){
        imprime_erro(res);
        *seq = (*seq+1)%16;
        return "";
    }

    *seq = (*seq+1)%16;

    // envia os dados referente ao número da linha
    msg = monta_mensagem("linha_dados", linha, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) == "nack" || 
            string_mensagem(res->tipo) != "conteudo" || 
            *seq != res->seq);

    do{

        if(string_mensagem(res->tipo) == "conteudo" && *seq == res->seq){
            while(!checa_paridade(res)){
                struct mensagem *nack;
                nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
                envia_mensagem(soquete, nack);
                do{
                    res = espera_mensagem(soquete, SERVIDOR, *seq);
                }while(string_mensagem(res->tipo) != "conteudo" || *seq != res->seq);
            }

            msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
            envia_mensagem(soquete, msg);
            *seq = (*seq + 1) % 16;

            for(int i = 0; i < res->tam ; i++)
                linha_res.push_back(res->dados[i]);
        }

        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);

    while(!checa_paridade(res)){
        struct mensagem *nack;
        nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
        envia_mensagem(soquete, nack);
        do{
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        }while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    }
    msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;

    return linha_res;
}

string linhas_cliente(int soquete, int *seq, string arquivo, string linha_inicial, string linha_final){
    struct mensagem *msg;
    struct mensagem *res;
    string linhas_res;
    
    msg = monta_mensagem("linhas", (char *)arquivo.c_str(), CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while((string_mensagem(res->tipo) != "ack" && 
             string_mensagem(res->tipo) != "erro") || 
            *seq != res->seq);

    if(string_mensagem(res->tipo) == "erro"){
        imprime_erro(res);
        *seq = (*seq+1)%16;
        return "";
    }
    *seq = (*seq+1)%16;

    string linhas = linha_inicial + ' ' + linha_final;
    msg = monta_mensagem("linha_dados", (char *)linhas.c_str(), CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) != "conteudo" || *seq != res->seq);

    do{
        if(string_mensagem(res->tipo) == "conteudo" && *seq == res->seq){
            while(!checa_paridade(res)){
                struct mensagem *nack;
                nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
                envia_mensagem(soquete, nack);
                do{
                    res = espera_mensagem(soquete, SERVIDOR, *seq);
                }while(string_mensagem(res->tipo) != "conteudo" || *seq != res->seq);
            }

            msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
            envia_mensagem(soquete, msg);
            *seq = (*seq + 1) % 16;

            for(int i = 0; i < res->tam ; i++)
                linhas_res.push_back(res->dados[i]);
        }

        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
        
    while(!checa_paridade(res)){
        struct mensagem *nack;
        nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
        envia_mensagem(soquete, nack);
        do{
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        }while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    }

    msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq + 1) % 16;

    return linhas_res;
}

void edit_cliente(int soquete, int *seq, string arquivo, string linha, string texto){
    struct mensagem *msg;
    struct mensagem *res;
    string edit_res, parte;
    int parte_tam, texto_tam;

    msg = monta_mensagem("edit", arquivo, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while((string_mensagem(res->tipo) != "ack" && 
             string_mensagem(res->tipo) != "erro") || 
            *seq != res->seq);

    if(res->tipo == 0b1111){
        imprime_erro(res);
        *seq = (*seq+1)%16;
        return;
    }

    *seq = (*seq + 1) % 16;

    msg = monta_mensagem("linha_dados", linha, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;

    texto_tam = texto.length()-1;

    for(int i = 1; i < texto_tam; i+=15){
        parte_tam = (texto_tam-i >= 15? 15 : texto_tam-i);
        parte = texto.substr(i, parte_tam);
        msg = monta_mensagem("conteudo", parte,   0b01, SERVIDOR, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        } while(res->tipo == 0b1001 || *seq != res->seq);

        *seq = (*seq + 1) % 16;			
    }
    
    do{
        msg = monta_mensagem("fim", "",   0b01, SERVIDOR, *seq);
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while(res->tipo == 0b1001 || *seq != res->seq);

    *seq = (*seq + 1) % 16;
}

string compilar_cliente(int soquete, int *seq, string arquivo, string opcoes){
    struct mensagem *msg, *res, *ack;
    string edit_res, parte, compilar_res;
    int parte_tam, opcoes_tam;

    msg = monta_mensagem("compilar", arquivo, CLIENTE, SERVIDOR, *seq);
    do{
        envia_mensagem(soquete, msg);
        res = espera_mensagem(soquete, SERVIDOR, *seq);
    } while((string_mensagem(res->tipo) != "ack" && 
             string_mensagem(res->tipo) != "erro") || 
            *seq != res->seq);

    if(string_mensagem(res->tipo) == "erro"){
        imprime_erro(res);
        *seq = (*seq+1)%16;
        return "";
    }
    *seq = (*seq + 1) % 16;
    // manda argumentos do compilar
    opcoes_tam = opcoes.length();
    for(int i = 0; i < opcoes_tam; i+=15){
        parte_tam = (opcoes_tam-i >= 15? 15 : opcoes_tam-i);
        parte = opcoes.substr(i, parte_tam);
        msg = monta_mensagem("conteudo", parte, CLIENTE, SERVIDOR, *seq);

        do{
            envia_mensagem(soquete, msg);
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        } while(string_mensagem(res->tipo) == "nack" || *seq != res->seq);

        *seq = (*seq + 1) % 16;			
    }
    
    msg = monta_mensagem("fim", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);

    // resultado do compilar vindo do servidor
    do{
        res = espera_mensagem(soquete, SERVIDOR, *seq);
        if(string_mensagem(res->tipo) != "conteudo" && *seq == res->seq){

            while(!checa_paridade(res)){
                struct mensagem *nack;
                nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
                envia_mensagem(soquete, nack);
                do{
                    res = espera_mensagem(soquete, SERVIDOR, *seq);
                }while(string_mensagem(res->tipo) != "conteudo" || *seq != res->seq);
            }

            ack = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
            envia_mensagem(soquete, ack);
            *seq = (*seq+1)%16;
        
            for(int i = 0; i < res->tam ; i++)
                compilar_res.push_back(res->dados[i]);
        }
    } while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    
    while(!checa_paridade(res)){
        struct mensagem *nack;
        nack = monta_mensagem("nack", "", CLIENTE, SERVIDOR, *seq);
        envia_mensagem(soquete, nack);
        do{
            res = espera_mensagem(soquete, SERVIDOR, *seq);
        }while(string_mensagem(res->tipo) != "fim" || *seq != res->seq);
    }
    msg = monta_mensagem("ack", "", CLIENTE, SERVIDOR, *seq);
    envia_mensagem(soquete, msg);
    *seq = (*seq+1)%16;

    return compilar_res;
}

#endif