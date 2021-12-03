#include <inttypes.h>
struct mensagem{
	uint8_t ini;
	uint8_t dst;
	uint8_t src;
	uint8_t tam;
	uint8_t seq;
	uint8_t tipo;
	uint8_t dados[15] = {0};
	uint8_t paridade;
};

uint8_t* monta_pacote(struct mensagem msg);

struct mensagem* desmonta_pacote(uint8_t *pacote);

void imprime_mensagem(struct mensagem msg);

int calcula_paridade(struct mensagem msg);

int tipo_mensagem(char *tipo);

char *string_mensagem(int b_tipo);

struct mensagem *monta_mensagem(char *tipo, char *dados, int src, int dst, int seq);
