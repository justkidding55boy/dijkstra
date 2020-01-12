#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//type
#define QUIT 0x01
#define PWD 0x02
#define CWD 0x03
#define LIST 0x04
#define RETR 0x05
#define STOR 0x06

#define CMD 0x10
#define CMDERR 0x11
#define FILEERR 0x12
#define UNKWNERR 0x13
#define DATA 0x20

struct commands {
    char *desc;
    int typenum;
    int codenum;
};

static struct commands cmdtbl[] = {
    {"QUIT", QUIT, -1},
    {"PWD", PWD, -1},
    {"CWD", CWD, -1},
    {"LIST", LIST, -1},
    {"RETR", RETR, -1},
    {"STOR", STOR, -1},

    {"CMD OK", CMD, 0x00},
    {"CMD OK, data lasts (s->c)", CMD, 0x01},
    {"CMD OK, data lasts (c->s)", CMD, 0x02},
    {"CMDERR syntax error", CMDERR, 0x01},
    {"CMDERR undefined error", CMDERR, 0x02},
    {"CMDERR protocol error", CMDERR, 0x03},
    {"FILEERR file doesn't exist", FILEERR, 0x00},
    {"FILEERR file permission denied", FILEERR, 0x01},
    {"UNKWNERR unknown error", UNKWNERR, 0x05},
    {"DATA data lasts", DATA, 0x00},
    {"DATA data end", DATA, 0x01},
    {NULL,  0, -1}
};


#define MAXCHAR 256

struct ftpmsg {
    uint8_t type;
    uint8_t  code;
    uint16_t datalen;
    char data[MAXCHAR];
};
