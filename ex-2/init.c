#include "buf.h"
#include <math.h>
#include <ctype.h>

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
		p->stat |= STAT_VALID;
		p->stat |= STAT_LOCKED;
		int h = hash(hash_list[i]);
		insert_bottom(&hash_head[h], p, 'b');
	}

	for (i = 0;  i < 6; i++) {
		int num = free_list[i];
		struct buf_header *p = hash_search(num);
		p->stat &= ~STAT_LOCKED;
		insert_bottom(&free_head, p, 'f');
	}
}
void buf_print_with_bufno(int bufno);
int get_bufno_with_blkno(int blkno);

void free_print()
{
	struct buf_header* p;
	printf("free list\n ");
	for (p = free_head.free_fp; p != &free_head; p = p->free_fp ) {
		int bufno = get_bufno_with_blkno(p->blkno);
		buf_print_with_bufno(bufno);
	}

	printf("\n");

}

void get_blkno_array(int *array)
{
	struct buf_header* p;
	int i;
	int cnt = 0;
	for (i = 0; i < NHASH; i++) {
		for (p = hash_head[i].hash_fp; p != &hash_head[i]; p = p->hash_fp ) {
			array[cnt++] = p->blkno;
		}
	}
	
}

void buf_print_with_bufno(int bufno)
{
	if (NHASH*BUFSIZE <= bufno || bufno < 0) {
		fprintf(stderr, "usage:buf 0 ~ %d\n", NHASH*BUFSIZE-1);
		return ;
	}

	int stat_array[6];
	char stat_char[6] = {'O', 'W', 'K', 'D', 'V', 'L'};

	int i;
	for (i = 0; i < 6; i++) {
		stat_array[5-i] = (int)pow(2, i);
	}


	int blkno_array[NHASH*BUFSIZE];
	get_blkno_array(blkno_array);
	int blkno = blkno_array[bufno];

	struct  buf_header* p = hash_search(blkno);
	printf("[%2d: %2d ", bufno, blkno);

	int j;
	for (j = 0; j < 6; j++) {
		if ((stat_array[j] & p->stat) == stat_array[j]) {
			printf("%c", stat_char[j]);
		} else {
			printf("-");
		}
	}

	printf("]");


}

void buf_print()
{
	int i;
	for (i = 0; i < NHASH*BUFSIZE; i++) {
		buf_print_with_bufno(i);
		printf("\n");
	}
	
}

int get_bufno_with_blkno(int blkno)
{
	int blkno_array[NHASH*BUFSIZE];
	get_blkno_array(blkno_array);
	int cnt;
	for (cnt = 0; cnt < NHASH*BUFSIZE; cnt++) {
		if (blkno_array[cnt] == blkno) {
			break;
		}
	}

	return cnt;
}

void hash_print_with_hash(int hash)
{
	if (0 > hash || hash > NHASH) {
		fprintf(stderr, "hash value: 0 ~ %d\n", NHASH);
		return ;
	}

	struct buf_header* p;

	printf("%d: ", hash);
	for (p = hash_head[hash].hash_fp; p != &hash_head[hash]; p = p->hash_fp ) {
		buf_print_with_bufno(get_bufno_with_blkno(p->blkno));
		printf(" ");
		//printf("%d ", p->blkno);
	}
	printf("\n");

}

int sisdigit(char *string)
	//true: not 0
	//false: 0
{
	int len = strlen(string);
	int i;
	int result = 1;
	for (i = 0; i < len && result; i++) {
		result = isdigit(*(string + i));
	}

	return result;
}

void buf_print_with_argv(int argc, char *argv[])
{
	int i, errcnt = 0;
	for (i = 1; i < argc; i++) {
		if (sisdigit(argv[i])) {
			buf_print_with_bufno(atoi(argv[i]));
			printf("\n");	
		} else {
			errcnt == 0 ?
				fprintf(stderr, "The argument should be in the proper range\n"):1;
			errcnt++;
		}

	}
}

void hash_print_with_argv(int argc, char *argv[])
{
	int i, errcnt = 0;
	for (i = 1; i < argc; i++) {
		if (sisdigit(argv[i])) {
			int a = atoi(argv[i]);
			if (0 <=a && a < NHASH) {
				hash_print_with_hash(atoi(argv[i]));
			} else {
				fprintf(stderr, "illegal hash value: %d\n", a);
			}
		} else {
			errcnt == 0 ?
				fprintf(stderr, "The argument should be the hash\n"):1;
			errcnt++;
		}
	}
}

void status_set_reset(int blkno, char stat, char type)
{// set the stat to the blkno

	struct buf_header *p = hash_search(blkno);
	if (p == NULL) {
		fprintf(stderr, "blkno not found\n");
		return ;
	}

	int stat_array[6];
	char stat_char[6] = {'O', 'W', 'K', 'D', 'V', 'L'};

	int i;
	for (i = 0; i < 6; i++) {
		stat_array[5-i] = (int)pow(2, i);
	}

	for (i = 0; i < sizeof(stat_char); i++) {
		if (stat_char[i] == stat) {
			if (type == 's') {
				p->stat |= stat_array[i]; 
			} else {
				p->stat &= ~stat_array[i]; 
			}
			break;
		}
		
	}

}


