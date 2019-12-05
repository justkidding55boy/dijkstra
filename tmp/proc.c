#include "buf.h"
#include <sys/param.h>
void quit_proc(int, char**);
void cd_proc(int, char**);
extern void fg_proc();
struct command_table cmd_tbl[] = {
	{"exit", quit_proc},
	{"quit", quit_proc},
	{"cd", cd_proc},
	{"fg", fg_proc},
	{NULL, NULL}
};
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


int proc_check(int argc, char **argv)
{
	int flg = 0;
	struct command_table *p;
	for (p = cmd_tbl; p->cmd; p++) {
		if (strcmp(argv[0], p->cmd) == 0) {
			(*p->func)(argc, argv);
			flg = 1;
			break;
		}
	}

	int i;
	if (flg == 1) {
		for (i = 0; i < MAX_ARGC; i++) {
			argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			free(argv[i]);
			argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
		}
	}
	return flg;

}


