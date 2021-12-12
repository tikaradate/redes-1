#ifndef __MISC__
#define __MISC__

#include <linux/if.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>

#include <string>
#include <fstream>

#include "misc.h"

using std::string;

int raw_socket(const char* dev){
    int soquete;
	struct ifreq ir;
	struct sockaddr_ll endereco;
	struct packet_mreq mr;
	
	soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  	/*cria socket*/
	if (soquete == -1) {
		printf("Erro no Socket\n");
		exit(-1);
	}

	memset(&ir, 0, sizeof(struct ifreq));  	/*dispositivo eth0*/
	memcpy(ir.ifr_name, dev, sizeof(dev));
	if (ioctl(soquete, SIOCGIFINDEX, &ir) == -1) {
		printf("Erro no ioctl %s\n", strerror(errno));
		exit(-1);
	}
		
	memset(&endereco, 0, sizeof(endereco)); 	/*IP do dispositivo*/
	endereco.sll_family = AF_PACKET;
	endereco.sll_protocol = htons(ETH_P_ALL);
	endereco.sll_ifindex = ir.ifr_ifindex;
	
	if (bind(soquete, (struct sockaddr *)&endereco, sizeof(endereco)) == -1) {
		printf("Erro no bind\n");
		exit(-1);
	}

	memset(&mr, 0, sizeof(mr));          /*Modo Promiscuo*/
	mr.mr_ifindex = ir.ifr_ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1)	{
		printf("Erro ao fazer setsockopt\n");
		exit(-1);
	}

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 1000;
	if (setsockopt(soquete, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) 
		perror("setsockopt failed");
	
    if (setsockopt(soquete, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
        perror("setsockopt failed\n");

    return soquete;
}

string checa_arquivo(string arquivo){
	// arquivo não existe
    if(access(arquivo.c_str(), F_OK ) != 0 ) {
        return "3";
    // não ter permissão de leitura   
    }else if (access(arquivo.c_str(), R_OK) != 0) {
        return "1";
    }
	return "";
}

string ls(const char *dir)
{
	struct dirent *d;
	string s;
	DIR *dh = opendir(dir);
	if (!dh)
	{
		if (errno == ENOENT){
			perror("Directory doesn't exist");
        } else {
			perror("Unable to read directory");
		}
		exit(EXIT_FAILURE);
	}

	while ((d = readdir(dh)) != NULL){
		s.append(d->d_name);
		s.push_back(' ');
	}
    return s;
}

int conta_linhas(string arquivo){
    std::ifstream arq (arquivo);   
    int linhas = 0;
    string linha;
    while (std::getline(arq, linha))
        linhas++;

    return linhas;
}

#endif