#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using std::string;

using namespace std;

int main(){
    string arquivo = "mensagem.cpp";
    ifstream myfile(arquivo);
    string line;
    int i = 1;
    int n_linha = 58;
    while(getline(myfile, line) && i < n_linha){
        i++;
    }
    cout << line << endl;
}