#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STAT_LOCKED		0x00000001	/*Locked*/
#define STAT_VALID		0x00000002	/*Have Valid Data*/
#define STAT_DWR		0x00000004	/*Delayed written*/
#define STAT_KRDWR		0x00000008	/*The karnel is writing/reading*/
#define STAT_WAITED		0x00000010  /*waited by other processes*/
#define STAT_OLD		0x00000020	/*This data is old*/

#define NHASH 4
#define hash(h) ((h)%(NHASH))
#define BUFSIZE 3

//input
#define MAXCHAR 256
#define MAXCOM 10

struct buf_header {
	int blkno;					/*Logical Block Number*/
	struct buf_header *hash_fp; /*The forward hash queue pointer*/
	struct buf_header *hash_bp; /*The backward hash queue pointer*/
	unsigned int stat;			/*The status of the buffer*/
	struct buf_header *free_fp; /*The forward free list pointer*/
	struct buf_header *free_bp; /*The backward free list pointer*/
	char *cache_data; 			/*The pointer to the cache data*/
};

