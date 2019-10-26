#include "buf.h"


extern struct buf_header* getblk(int);

//init.c
extern void insert_to_initlist();
extern void free_print();

//main.c
extern struct buf_header hash_head[NHASH];
extern struct buf_header free_head;

int sisdigit(char*);
int getblk_proc(int argc, char *argv[]) 
{	
	if (argc > 2 || !sisdigit(argv[1])) {
		return 1;
	}
	
	struct buf_header* p = getblk(atoi(argv[1]));	
	printf("take out blkno:%d\n", p->blkno);
	return 0;
}

int quit_proc(int argc, char *argv[])
{
	if (argc > 1) {
		return 1;
	}
	printf("end of this process\n");
	exit(0);
	return 0;
}

int init_proc(int argc, char *argv[])
{
	if (argc > 1) {
		return 1;
	}
	int i;
	for (i = 0; i < NHASH; i++) {
		hash_head[i].hash_fp = hash_head[i].hash_bp = &hash_head[i];
	}

	free_head.free_fp = free_head.free_bp = &free_head;
	insert_to_initlist();
	return 0;
}



int free_proc(int argc, char *argv[])
{
	if (argc > 1) {
		return 1;
	}
	free_print();
	return 0;
}

extern void buf_print();
extern void buf_print_with_bufno();
void buf_print_with_argv(int argc, char *argv[]);
int buf_proc(int argc, char *argv[])
{
	if (argc == 1) {
		buf_print();
	} else if (2 <= argc) {
		buf_print_with_argv(argc, argv);
	}
	return 0;
}

extern struct command_table cmd_tbl;
int help_proc(int argc, char *argv[])
{
	if (argc > 1) {
		return 1;
	}

	struct command_table *p;
	for (p = &cmd_tbl; p->cmd; p++) {
		printf("%-19s:%s\n", p->format, p->desc);
	}

	return 0;

}
extern void hash_print_with_hash(int hash);
extern void hash_print_with_argv(int argc, char *argv[]);

int hash_proc(int argc, char *argv[])
{
	if (argc == 1) {
		int i;
		for (i = 0; i < NHASH; i++) {
			hash_print_with_hash(i);
		}
	} else {
		hash_print_with_argv(argc, argv);
	}
	return 0;
}

extern void status_set(int blkno, char stat);
int set_proc(int argc, char *argv[])
{

	if (argc <= 2) {
		return 1;
	}
	status_set(atoi(argv[1]), argv[2][0]);
	return 0;

}
