struct mensagem{
	int ini;
	int dst;
	int src;
	int tam;
	int seq;
	int tipo;
	int dados; // um vetor talvez?
	int paridade;
};

int calcula_paridade(struct mensagem msg);


