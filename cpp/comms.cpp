#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "comms.h"

int raw_socket(char *dev){
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
	printf("%d %d %d\n", endereco.sll_family, endereco.sll_protocol, endereco.sll_ifindex);
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

	// struct timeval tv;
	// tv.tv_sec = 0;
	// tv.tv_usec = 1000;
	// if (setsockopt(soquete, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) 
	// 	perror("Error");
	
    // if (setsockopt(soquete, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0)
    //     perror("setsockopt failed\n");

    return soquete;
}