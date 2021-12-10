#include <string>

using std::string;

void cd_servidor (int soquete, int *seq, struct mensagem *msg);

void ls_servidor(int soquete, int *seq, struct mensagem *msg);

void ver_servidor(int soquete, int *seq, struct mensagem *msg);

void linha_servidor(int soquete, int *seq, struct mensagem *msg);

void linhas_servidor(int soquete, int *seq, struct mensagem *msg);

void edit_servidor(int soquete, int *seq, struct mensagem *msg);

void compilar_servidor(int soquete, int *seq, struct mensagem *msg);