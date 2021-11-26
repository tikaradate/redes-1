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

#include "comms.h"

#define DEV "lo"
int main(){
	char c[100];
	int bytes;
	int soquete;
	soquete = raw_socket("lo");
	// todos equivalentes
	// bytes = recvfrom(soquete, c, sizeof(c), 0, (struct sockaddr*) &endereco, &fromlen);
	// bytes = recv(soquete, c, sizeof(c), 0);
	bytes = read(soquete, c, sizeof(c));
	printf("recv()'d %d bytes of data in buf\n", bytes);
	printf("%s\n", c);
}