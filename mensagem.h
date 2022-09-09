#include <inttypes.h>
#include <string>

using std::string;

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

uint8_t* monta_pacote(struct mensagem *msg);

struct mensagem* desmonta_pacote(uint8_t *pacote);

void imprime_mensagem(struct mensagem *msg);

bool checa_paridade(struct mensagem *msg);

int tipo_mensagem(string tipo);

string string_mensagem(int tipo);

struct mensagem *monta_mensagem(string tipo, string dados, int src, int dst, int seq);

void envia_mensagem(int soquete, struct mensagem *msg);

struct mensagem *espera_mensagem(int soquete, int src, int seq);

void imprime_erro(struct mensagem *msg);