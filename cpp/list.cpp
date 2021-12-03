#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <iostream>

#include "list.h"

using std::string;

string ls(const char *dir)
{
	//Here we will list the directory
	struct dirent *d;
	string s;
	DIR *dh = opendir(dir);
	if (!dh)
	{
		if (errno = ENOENT){
			perror("Directory doesn't exist");
        } else {
			perror("Unable to read directory");
		}
		exit(EXIT_FAILURE);
	}
	//While the next entry is not readable we will print directory files
	while ((d = readdir(dh)) != NULL){
		//If hidden files are found we continue
		//if (!op_a && d->d_name[0] == '.') continue;
		s.append(d->d_name);
		s.push_back(' ');
		//if(op_l) printf("\n");
	}

	//if(!op_l) 
    return s;
}
// int main(){

// 	std::cout << ls(".") << std::endl; 
// 	chdir("teste");
//     std::cout << ls(".") << std::endl; 
// 	// char *exec_args[] = {"gcc", "teste.c", 0};
// 	// // execvp(exec_args[0], exec_args);
// 	// system("gcc teste.c");
// 	// _ls(".");
// 	return 0;
// }
