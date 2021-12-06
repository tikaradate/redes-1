#include <string>

using std::string;

void cd_cliente (int soquete, int *seq, string diretorio);

string ls_cliente(int soquete, int *seq);

string ver_cliente(int soquete, int *seq, string arquivo);

string linha_cliente(int soquete, int *seq, string arquivo, string linha);

string linhas_cliente(int soquete, int *seq, string arquivo, string linha_inicial, string linha_final);

void edit_cliente(int soquete, int *seq, string arquivo, string linha, string texto);

string compilar_cliente(int soquete, int *seq, string arquivo, string opcoes);