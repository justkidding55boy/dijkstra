#include <stdio.h>
#include <stdlib.h>
#include "buf.h"

extern struct buf_header hash_head[NHASH];
extern struct buf_header free_head;
//init.c
extern struct buf_header* make_card(int blkno);

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

void brelse(struct buf_header *buffer)
{
	//wake up all procs; event, waiting for any buffer to become free
	//wakeup();
	printf("wake up all procs; event, waiting for any buffer\n");
	
	//wake up all procs; event, waiting for this buffer to become free
	//wakeup();
	printf("wake up all procs; event, waiting for this buffer\n");

	//raise processor execution level to block interrupts
	//raise_cpu_level();


	if ((buffer->stat&STAT_VALID) == STAT_VALID && (buffer->stat&STAT_OLD) != STAT_OLD) {
		//enqueue the buffer at the end of the free list
		insert_bottom(&free_head, buffer, 'f');
		printf("insert the buffer to the bottom of the free list\n");
	} else {
		insert_head(&free_head, buffer, 'f');
		printf("insert the buffer to the head of the free list\n");
		buffer->stat &= ~STAT_OLD;
		buffer->stat &= ~STAT_DWR;
	}
	//lower_cpu_level();
	//unlock
	buffer->stat &= ~STAT_LOCKED;
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
				printf("scenario 5\n");
				// sleep();
				printf("Process goes to sleep\n");
				return NULL;
			}
			/*scenario 1*/
			printf("scenario 1\n");
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
			replaced_buffer->stat |= STAT_LOCKED;
			remove_buffer(replaced_buffer, 'f');

			if ((replaced_buffer->stat & STAT_DWR) == STAT_DWR) {
			//if the buffer marked for delayed writing
				/*scenario 3*/
				printf("scenario 3\n");
				//asynchronous write buffer to disk;
				p = free_head.free_fp;
				replaced_buffer->stat |= STAT_LOCKED;
				replaced_buffer->stat |= STAT_OLD;
				continue;
			}
			/*scenario 2*/
			printf("scenario 2\n");
			//remove the buffer from the old hash queue;
			remove_buffer(replaced_buffer, 'b');

			//reset VALID and Replace it with the new buffer
			replaced_buffer->stat &= ~STAT_VALID;

			//put the buffer on the new hash queue;
			int h = hash(blkno);
			p = replaced_buffer;

			//load the data from HDD
			p->stat |= STAT_KRDWR;
			p->blkno = blkno;

			//finish the loading
			p->stat &= ~STAT_KRDWR;
			insert_bottom(&hash_head[h], p, 'b');
			p->stat |= STAT_VALID;	
			//return the pointer to the buffer;
			return p;
		}
	} while (p->blkno != blkno); 
		//while buffer not found

	return NULL;
}
