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

#include "mensagem.h"
#include "comms.h"

int main(){
	int soquete;

	soquete = raw_socket("lo");
	char b[100] = "123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789\n";
	if(send(soquete, b, sizeof(b), MSG_OOB))
		perror("AAAAAA");
	// if(sendto(soquete, b, sizeof(b), 0, (struct sockaddr*) &endereco, sizeof(struct sockaddr_ll)))
	// 	perror("AAAAAA");
}