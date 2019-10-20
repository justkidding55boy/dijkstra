#include <stdio.h>
#include <stdlib.h>
#include "buf.h"

extern struct buf_header {
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


void insert_head(struct buf_header *h, struct buf_header *p, char type)
{
	//input h: the pointer to the list head
	//		p: the pointer to the inserting buffer
	//	 Type: The type of the list (buffer or free)
	if (type == 'b') {
		p->hash_bp = h;
		p->hash_fp = h->hash_fp;
		h->hash_fp->hash_bp = p;
		h->hash_fp = p;
	} else if (type ==  'f') {
		p->free_bp = h;
		p->free_fp = h->free_fp;
		h->free_fp->free_bp = p;
		h->free_fp = p;
	} else {
		printf("Select the type 'b' or 'f'\n");
	}
}

void insert_bottom(struct buf_header *h, struct buf_header *p, char type)
{
	//input h: the pointer to the list head
	//		p: the pointer to the inserting buffer
	//	 Type: The type of the list (buffer or free)
	if (type == 'b') {
		p->hash_bp = h->hash_bp;
		p->hash_fp = h;
		h->hash_bp->hash_fp = p;
		h->hash_bp = p;
	} else if (type == 'f') {
		p->free_bp = h->free_bp;
		p->free_fp = h;
		h->free_bp->free_fp = p;
		h->free_bp = p;
	} else {
		printf("Select the type 'b' or 'f'\n");
		printf("type:%c\n", type);
	}

}

void remove_buffer(struct buf_header *p, char type)
{
	//input p: the pointer to the removing buffer
	//   type: The type of the list (buffer or free)
	if (type == 'b') {
		p->hash_bp->hash_fp = p->hash_fp;
		p->hash_fp->hash_bp = p->hash_bp;
		p->hash_fp = p->hash_bp = NULL;
	} else if (type == 'f') {
		p->free_bp->free_fp = p->free_fp;
		p->free_fp->free_bp = p->free_bp;
		p->free_fp = p->free_bp = NULL;
	}
}

struct buf_header* hash_search(int blkno)
{
	//input: the target block number
	//output: the pointer to the buffer
	//if not, the return value is NULL
	int h;
	struct buf_header *p;

	h = hash(blkno);
	for (p = hash_head[h].hash_fp; p != &hash_head[h]; p = p->hash_fp) {
		if (p->blkno == blkno) {
			return p;
		}
	}

	return NULL;

}


struct buf_header* getblk(int blkno)
{ 	//input: The target Logical Block Number
	//output: the pointer for the locked buf_header
	struct buf_header *p = NULL;
	do {
		if ((p = hash_search(blkno)) != NULL) {
		//if buffer in the hash queue	
			if ((p->stat & STAT_LOCKED) == STAT_LOCKED) {
			//if the buffer is locked	
				/*scenario 5*/
				// sleep();
				printf("Process goes to sleep\n");
				return NULL;
			}
			/*scenario 1*/
			//make the buffer locked;
			p->stat |= STAT_LOCKED;

			//remove the buffer from the free list
			remove_buffer(p, 'f');
			
			//return the pointer to the buffer
			return p;
		} else {
			if (free_head.free_fp == &free_head) {
			//if no buffer in the free list
				/*scenario 4*/
				//sleep();
				printf("scenario 4\n");
				printf("Process goes to sleep\n");
				return NULL;
			}

			//remove the top buffer from the free list;
			struct buf_header *replaced_buffer = free_head.free_fp;
			remove_buffer(replaced_buffer, 'f');
			if ((replaced_buffer->stat & STAT_DWR) == STAT_DWR) {
			//if the buffer marked for delayed writing
				/*scenario 3*/
				//asynchronous write buffer to disk;
				continue;
			}
			/*scenario 2*/
			//remove the buffer from the old hash queue;
			remove_buffer(replaced_buffer, 'b');
			//put the buffer on the new hash queue;
			int h = hash(blkno);
			p = malloc(sizeof(struct buf_header));
			insert_bottom(&hash_head[h], p, 'b');
			
			//return the pointer to the buffer;
			return p;
		}
	} while (p->blkno != blkno); 
		//while buffer not found

}
