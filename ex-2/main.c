//61706613 Eiji Kudo
#include <stdio.h>
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


int main()
{
	int i;
	for (i = 0; i < NHASH; i++) {
		hash_head[i].hash_fp = hash_head[i].hash_bp = &hash_head[i];
	}

	free_head.free_fp = free_head.free_bp = &free_head;
	printf("finish\n");



	return 0;
}
