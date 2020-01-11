#include <ctype.h>
#include <stdio.h>
#include <error.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>

//new

#define SDISCOVER  1
#define RDISCOVER 1
#define SOFFER 2
#define ROFFER 2
#define SREQUEST  3
#define RREQUEST 3
#define SACK 4
#define RACK 4
#define SRELEASE 5
#define RRELEASE 5


//server
#define INIT 6
#define DISCOVER_WAIT 7
#define REQUEST_WAIT 8
#define IP_ALLOC 9
#define FINISH 10

#define RELEASE 11
#define NONE 12
#define IPSTARVE 13
#define REJECT 14
#define SSTARVE 15

#define OFFER_WAIT 16

#define IPGET 17
#define TIMEHALF 18
#define RELEASEPREP 19
#define ACK_WAIT 20



#define MAXCHAR 512
#define TIMEOUT 10
static char *mytypes[MAXCHAR] = 
{
    "DISCOVER",
    "OFFER",
    "REQUEST",
    "ACK",
    "RELEASE",
    "INIT",
    "DISCOVER_WAIT",
    "REQUEST_WAIT",
    "IP_ALLOC",
    "FINISH",

    "RELEASE",
    "NONE",
    "IPSTARVE",
    "REJECT",
    "SSTARVE",

    "OFFER_WAIT",

    "IPGET",
    "TIMEHALF",
    "RELEASEPREP",
    "ACK_WAIT",
    NULL
};

struct client {
    struct client *fp;
    struct client *bp;
    short status;
    int ttlcounter;
    // bellow : network byte order
    struct in_addr id;
    struct in_addr addr;
    u_short port;
    struct in_addr netmask;
    uint16_t ttl;
    uint16_t rttl; //remaining time
    //struct time val // 再送のための送信時刻
} client_list;

struct client client_list;

struct dhcph {
    uint8_t type; //0~8
    uint8_t code; // 0 or 1
    uint16_t ttl;
    in_addr_t address;
    in_addr_t netmask;
};
