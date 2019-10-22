#include "buf.h"


extern struct buf_header* getblk(int);

//init.c
extern void insert_to_initlist();
extern void hash_print();
extern void free_print();

//main.c
extern struct buf_header hash_head[NHASH];
extern struct buf_header free_head;

void getblk_proc(int argc, char *argv[]) 
{	
	struct buf_header* p = getblk(atoi(argv[1]));	
	printf("take out blkno:%d\n", p->blkno);
}

void quit_proc(int argc, char *argv[])
{
	printf("end of this process\n");
	exit(0);
}

void init_proc(int argc, char *argv[])
{
	int i;
	for (i = 0; i < NHASH; i++) {
		hash_head[i].hash_fp = hash_head[i].hash_bp = &hash_head[i];
	}

	free_head.free_fp = free_head.free_bp = &free_head;
	insert_to_initlist();
}

void hash_proc(int argc, char *argv[])
{
	if (argc != 1) {
		int h = atoi(argv[1]);
		if (!(0 <= h && h < NHASH)) {
			fprintf(stderr, "useage:hash 0 ~ %d\n", NHASH);
			hash_print();
			return ;
		}
		
		printf("hash[%d] ", h);
		struct buf_header *p;
		for (p = hash_head[h].hash_fp; p != &hash_head[h]; p = p->hash_fp ) {
			printf("%d ", p->blkno);
		}
		printf("\n");
	} else {
		hash_print();
	}
		
}

void free_proc()
{
	free_print();
}
