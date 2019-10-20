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

extern struct buf_header hash_head[NHASH];
extern struct buf_header free_head;

extern struct buf_header* getblk(int);
extern void insert_bottom(struct buf_header *h, struct buf_header *p,
		char type);

struct buf_header* make_freecard()
{
	struct buf_header *p = malloc(sizeof(struct buf_header));
	p->stat = 0;
	static int cnt = 0;
	printf("blk:%d\n", cnt);
	p->blkno = cnt++;
	return p;
}

void insert_freecard()
{
	int i;
	for (i = 0; i < NHASH; i++) {
		int j;
		for (j = 0; j < BUFSIZE; j++) {
			struct buf_header* p = make_freecard();
			insert_bottom(&hash_head[i], p, 'b');
			insert_bottom(&free_head, p, 'f');
		}
	}
}

void hash_print()
{
	struct buf_header* p;
	printf("print hash table\n");
	int i;
	for (i = 0; i < NHASH; i++) {
		printf("hash[%d] ", i );
		for (p = hash_head[i].hash_fp; p != &hash_head[i]; p = p->hash_fp ) {
			printf("%d ", p->blkno);
		}
		printf("\n");
	}

	printf("free list\n ");
	for (p = free_head.free_fp; p != &free_head; p = p->free_fp ) {
		printf("%d ", p->blkno);
	}

}



