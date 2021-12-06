#include <string>

using std::string;

void cd_servidor (int soquete, int *seq, string diretorio);

void ls_servidor(int soquete, int *seq);

void ver_servidor(int soquete, int *seq, string arquivo);

void linha_servidor(int soquete, int *seq, string arquivo, string linha);

void linhas_servidor(int soquete, int *seq, string arquivo, string linha_inicial, string linha_final);

void edit_servidor(int soquete, int *seq, string arquivo, string linha, string texto);

void compilar_servidor(int soquete, int *seq, string arquivo, string opcoes);