#include "buf.h"
#include <sys/param.h>
void quit_proc(int argc, char **argv)
{
	exit(0);
}

void cd_proc(int argc, char **argv)
{

	if (argc == 1) {
		char *path = getenv("HOME");
		chdir(path);
	} else if (argc == 2) {
		if (strcmp(argv[1], "~") == 0) {
			char *path = getenv("HOME");
			chdir(path);
		} else {
			if (chdir(argv[1]) < 0) {
				perror("cd");
			}
		} 
	}
}


void red_do()
{

}
