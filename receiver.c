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

#define DEV "lo"
int main(){
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
	memcpy(ir.ifr_name, DEV, sizeof(DEV));
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

	char c[100];
	int bytes, fromlen = sizeof(endereco);
	bytes = recvfrom(soquete, c, sizeof(c), 0, (struct sockaddr*) &endereco, &fromlen);
	printf("recv()'d %d bytes of data in buf\n", bytes);
	printf("%s\n", c);
	printf("from sll addr %d\n", inet_ntoa(endereco.sll_addr)); 
}