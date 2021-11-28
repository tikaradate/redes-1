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

#include <inttypes.h> 

#include "comms.h"
#include "mensagem.h"

#define DEV "lo"
int main(){
	uint8_t pacote[19];
	int bytes;
	int soquete;
	soquete = raw_socket("lo");
	// todos equivalentes
	// bytes = recvfrom(soquete, c, sizeof(c), 0, (struct sockaddr*) &endereco, &fromlen);
	// bytes = recv(soquete, c, sizeof(c), 0);
	bytes = read(soquete, pacote, 20);
	printf("recv()'d %d bytes of data in buf\n", bytes);
	printf("AAAAAA\n");
	printf("%d\n", pacote[0]);
	printf("%d\n", pacote[1]);
	printf("%d\n", pacote[2]);
	struct mensagem *msg = desmonta_pacote(pacote);

	imprime_mensagem(*msg);
}