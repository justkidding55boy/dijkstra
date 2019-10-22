//61706613 Eiji Kudo
#include "buf.h"

struct buf_header hash_head[NHASH];
struct buf_header free_head;

//getblk.c
extern struct buf_header* getblk(int);

//init.c
extern void hash_print();
extern void free_print();
extern void insert_to_initlist();

//input_line.c
extern void getargs(int *argc, char *argv[]);

//proc.c
extern void getblk_proc();
extern void quit_proc();
extern void init_proc();
extern void hash_proc();
extern void free_proc();

struct command_table {
	char *cmd;
	void (*func)(int, char *[]);
} cmd_tbl[] = {
	{"getblk", getblk_proc},
	{"quit", quit_proc},
	{"init", init_proc},
	{"hash", hash_proc},
	{"free", free_proc},
	{NULL, NULL}
};

int main()
{
	//initialization process


	//insert the initial cards
	//insert_freecard();

	init_proc();	
	hash_print();
	free_print();

	while(1)  {
		int my_argc;
		char **my_argv;
		my_argv = malloc(sizeof (char *) * MAXCOM);

		my_argc = 0;
		int i;
		for (i = 0; i < MAXCOM; i++) {
			my_argv[i] = malloc(sizeof (char) * MAXCHAR);
		}

		getargs(&my_argc, my_argv);

		/*
		for (i = 0; i < my_argc; i++) {
			printf("argv[%d]: %s\n", i, my_argv[i]);
		}*/

		struct command_table *p;
		for (p = cmd_tbl; p->cmd; p++) {
			if (strcmp(my_argv[0], p->cmd) == 0) {
				(*p->func)(my_argc, my_argv);
				break;
			}
		}



	}


	return 0;
}
