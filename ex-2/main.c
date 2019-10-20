//61706613 Eiji Kudo
#include <stdio.h>
#include <stdlib.h>
#include "buf.h"

struct buf_header {
	int blkno;					/*Logical Block Number*/
	struct buf_header *hash_fp; /*The forward hash queue pointer*/
	struct buf_header *hash_bp; /*The backward hash queue pointer*/
	unsigned int stat;			/*The status of the buffer*/
	struct buf_header *free_fp; /*The forward free list pointer*/
	struct buf_header *free_bp; /*The backward free list pointer*/
	char *cache_data; 			/*The pointer to the cache data*/
};

struct buf_header hash_head[NHASH];
struct buf_header free_head;

extern struct buf_header* getblk(int);
extern void insert_bottom(struct buf_header *h, struct buf_header *p,
		char type);

extern void insert_freecard();
extern void hash_print();

int main()
{
	//initialization process
	int i;
	for (i = 0; i < NHASH; i++) {
		hash_head[i].hash_fp = hash_head[i].hash_bp = &hash_head[i];
	}

	free_head.free_fp = free_head.free_bp = &free_head;

	//insert the initial cards
	insert_freecard();

	hash_print();

	printf("input:\n");
	char *input;
	scanf("%s", input);
	int num = atoi(input);

	getblk(num);
	printf("finish\n");



	return 0;
}
