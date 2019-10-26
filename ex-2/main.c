//61706613 Eiji Kudo
#include "buf.h"

struct buf_header hash_head[NHASH];
struct buf_header free_head;

//getblk.c
extern struct buf_header* getblk(int);

//init.c

extern void insert_to_initlist();

//input_line.c
extern void getargs(int *argc, char *argv[]);

//proc.c
extern int getblk_proc();
extern int quit_proc();
extern int init_proc();
extern int hash_proc();
extern int free_proc();
extern int buf_proc();
extern int help_proc();
extern int set_proc();
extern int reset_proc();

struct command_table cmd_tbl[] = {
	{"getblk", getblk_proc, "getblk n", "do getblk(n) with blkno n"},
	{"quit", quit_proc, "quit", "quit this process"},
	{"init", init_proc, "init", "initialize the hash list and the free list"},
	{"hash", hash_proc, "hash or hash [n...]", 
		"display the entire hash list without the argument or the hash listswith the designated hash numbers. The argument n is the wanted hash value"},
	{"free", free_proc, "free", "display the free list"},
	{"buf", buf_proc, "buf or buf [n...]", "Display the designated buf status with the arguments. Without arguments, show all the buf status"},
	{"help", help_proc, "help", "Show the command information"},
	{"set", set_proc, "set n stat [stat...]", "set the status to the buffer of the blkno n"},
	{"reset", reset_proc, "reset n stat [stat...]", "reset the status to the buffer of the blkno"},
	{NULL, NULL}
};

int main()
{
	//initialization process

	init_proc();

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
		} */

		int err=0;
		struct command_table *p;
		for (p = cmd_tbl; p->cmd; p++) {
			if (strcmp(my_argv[0], p->cmd) == 0) {
				err = (*p->func)(my_argc, my_argv);
				break;
			}
		}

		if (err == 1) {
			struct command_table *t;
			for (t = cmd_tbl; t->cmd; t++) {	
				if (strcmp(t->cmd, my_argv[0]) == 0) {
					fprintf(stderr, "usage: %s\ndescription: %s\n", t->format, t->desc);
				}
			}
		}

		if (p->cmd == NULL) {
			fprintf(stderr, "%s: command not found\n", my_argv[0]);
		}

	}


	return 0;
}
