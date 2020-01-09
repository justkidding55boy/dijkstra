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


//server
#define INIT 1
#define DISCOVER_WAIT 2
#define REQUEST_WAIT 3
#define IP_ALLOC 4
#define FINISH 5

#define RDISCOVER 6
#define ROFFER 7
#define RREQUEST 8

#define SDISCOVER  9
#define SOFFER 10
#define SACK 11
#define SREQUEST  12
#define RELEASE 13
#define NONE 14
#define IPSTARVE 15
#define REJECT 16
#define SSTARVE 17

#define OFFER_WAIT 18
#define RACK 19
#define IPGET 20
#define TIMEHALF 21
#define RELEASEPREP 22
#define ACK_WAIT 23

#define MAXCHAR 512
#define TIMEOUT 10
static char *mytypes[MAXCHAR] = 
{
    "INIT",
    "DISCOVER_WAIT",
    "REQUEST_WAIT",
    "IP_ALLOC",
    "FINISH",

    "RDISCOVER",
    "ROFFER",
    "RREQUEST",

    "SDISCOVER",
    "SOFFER",
    "SACK",
    "SREQUEST",
    "RELEASE",
    "NONE",
    "IPSTARVE",
    "REJECT",
    "SSTARVE",
    "OFFER_WAIT",
    "RACK",
    "IPGET",
    "TIMEHALF",
    "RELEASEPREP",
    "ACK_WAIT",
    NULL
};

struct client
{
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
