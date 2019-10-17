#define STAT_LOCKED		0x00000001	/*Locked*/
#define STAT_VALID		0x00000002	/*Have Valid Data*/
#define STAT_DWR		0x00000004	/*Delayed written*/
#define STAT_KRDWR		0x00000008	/*The karnel is writing/reading*/
#define STAT_WAITED		0x00000010  /*waited by other processes*/
#define STAT_OLD		0x00000020	/*This data is old*/

#define NHASH 4
#define hash(h) ((h)%(NHASH))
