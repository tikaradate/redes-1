struct mensagem{
	int ini;
	int dst;
	int src;
	int tam;
	int seq;
	int tipo;
	int dados[15];
	int paridade;
};

int* monta_pacote(struct mensagem msg);

int calcula_paridade(struct mensagem msg);