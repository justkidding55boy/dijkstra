#include <stdio.h>
#include "buf.h"
struct buf_header* getblk(int blkno)
{ 	//input: The target Logical Block Number
	//output: the pointer for the locked buf_header
	struct buf_header *p = NULL;
	while (p->blkno != blkno) {
		//while buffer not found
		if ((p = hash_search(blkno) != NULL)) {
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
			p->stat |= STAT_LOCKED

			//remove the buffer from the free list
			remove_buffer(p, 'f');
			
			//return the pointer to the buffer
			return p;
		} else {
			if (free_head->free_fp == free_head) {
			//if no buffer in the free list
				/*scenario 4*/
				//sleep();
				printf("Process goes to sleep\n");
				return NULL;
			}

			//remove the top buffer from the free list;
			struct buf_header *replaced_buffer = free_head->free_fp;
			remove_buffer(replaced_buffer, 'f');
			if ((replaced_buffer->stat & STAT_DWR) == STAT_DWR) {
			//if the buffer marked for delayed writing
				/*scenario 3*/
				//asynchronous write buffer to disk;
				continue;
			}
			/*scenario 2*/
			//remove the buffer from the old hash queue;
			remove_buffer(replaced_buffer, 'h');
			//put the buffer on the new hash queue;
			insert_bottom(p, 'h');
			
			//return the pointer to the buffer;
			return p;
		}
	}

}
