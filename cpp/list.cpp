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

	while ((d = readdir(dh)) != NULL){
		s.append(d->d_name);
		s.push_back(' ');
	}
    return s;
}