#include <linux/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#include <math.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <inttypes.h>

#include <iostream>
#include <fstream>
#include <string>

#include "misc.h"
#include "mensagem.h"
#include "funcoes_servidor.h"

using std::cout;
using std::endl;
using std::string;

int main(){
	int soquete;
	soquete = raw_socket("lo");

	int seq = 0;
	while(true){
		struct mensagem *res, *nack;

		res = espera_mensagem(soquete, 0b01, seq);
		if(!checa_paridade(res)){
			nack = monta_mensagem("nack", "", 0b10, 0b01, seq);
        	envia_mensagem(soquete, nack);
		}
		if(seq != res->seq){
			continue;
		}
		string comando = string_mensagem(res->tipo);
		if(!comando.empty())
			cout << comando << endl;
		if(comando == "cd"){
			cd_servidor(soquete, &seq, res);
		} else if(comando == "ls"){
			ls_servidor(soquete, &seq, res);
		} else if(comando == "ver"){
			ver_servidor(soquete, &seq, res);
		} else if(comando == "linha"){
			linha_servidor(soquete, &seq, res);
		} else if(comando == "linhas"){
			linhas_servidor(soquete, &seq, res);
		} else if(comando == "edit"){
			edit_servidor(soquete, &seq, res);
		} else if(comando == "compilar"){
			compilar_servidor(soquete, &seq, res);
		}
	}
}