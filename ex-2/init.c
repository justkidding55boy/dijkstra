#include <stdio.h>
#include <stdlib.h>
#include "buf.h"

extern struct buf_header hash_head[NHASH];
extern struct buf_header free_head;

extern struct buf_header* getblk(int);
extern void insert_bottom(struct buf_header *h, struct buf_header *p,
		char type);

//getblk.c
extern struct buf_header *hash_search(int);


struct buf_header* make_card(int blkno) {
	struct buf_header *p = malloc(sizeof(struct buf_header));
	p->stat = 0;
	p->blkno = blkno;
	p->hash_fp = p->hash_bp = p->free_fp = p->free_bp = NULL;
	
	return p;
}

void insert_to_initlist()
{
	int hash_list[NHASH * BUFSIZE] = {28, 4, 64, 17, 5, 97, 98, 50, 10, 3, 35, 99};

	int free_list[6] = {3, 5, 4, 28, 97, 10};
	int i;
	for (i = 0; i < NHASH * BUFSIZE; i++) {
		struct buf_header *p = make_card(hash_list[i]);
		int h = hash(hash_list[i]);
		insert_bottom(&hash_head[h], p, 'b');
	}

	for (i = 0;  i < 6; i++) {
		int num = free_list[i];
		struct buf_header *p = hash_search(num);

		insert_bottom(&free_head, p, 'f');
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
}

void free_print()
{
	struct buf_header* p;
	printf("free list\n ");
	for (p = free_head.free_fp; p != &free_head; p = p->free_fp ) {
		printf("%d ", p->blkno);
	}

	printf("\n");

}



