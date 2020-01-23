#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

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
    void (*func)(int dstSocket, char *data, int datalen); //server_func
};

void spwd_proc(int dstSocket);
void scd_proc(int dstSocket, char *buf, int buflen);
void slist_proc(int dstSocket, char *path, int pathlen);
void sget_proc(int dstSocket, char *filename, int datalen);
void sput_proc(int dstSocket, char *filename, int datalen);
void squit_proc(int dstSocket, char *filename, int datalen);

static struct commands cmdtbl[] = {
    {"QUIT", QUIT, -1, squit_proc},
    {"PWD", PWD, -1, spwd_proc},
    {"CWD", CWD, -1, scd_proc},
    {"LIST", LIST, -1, slist_proc},
    {"RETR", RETR, -1, sget_proc},
    {"STOR", STOR, -1, sput_proc},

    {"CMD OK", CMD, 0x00},
    {"CMD OK, data lasts (s->c)", CMD, 0x01},
    {"CMD OK, data lasts (c->s)", CMD, 0x02},
    {"CMDERR syntax error", CMDERR, 0x01},
    {"CMDERR undefined error", CMDERR, 0x02},
    {"CMDERR protocol error", CMDERR, 0x03},
    {"FILEERR file doesn't exist", FILEERR, 0x00},
    {"FILEERR file permission denied", FILEERR, 0x01},
    {"UNKWNERR unknown error", UNKWNERR, 0x05},
    {"DATA data end", DATA, 0x00},
    {"DATA data lasts", DATA, 0x01},
    {NULL,  0, -1, 0}
};



void quit_proc(), pwd_proc(), cd_proc(),
dir_proc(), lpwd_proc(), lcd_proc(), ldir_proc(),
get_proc(), put_proc(), help_proc();

struct cmds {
    char *cmd;
    void (*func)(int dstSocket, char *av[], int ac);
    char *desc;
};
static struct cmds cmdtab [] = {
    {"quit", quit_proc, ": Finish myftpc"},
    {"pwd", pwd_proc, ": Display the server current directory"},
    {"cd", cd_proc, " path: Change the server directory"},
    {"dir", dir_proc, " [path] : Display the file information"},
    {"lpwd", lpwd_proc, ": Display the client current directory"},
    {"lcd", lcd_proc, " path: Change the client directory"},
    {"ldir", ldir_proc, " [path]: Display the client file information"},
    {"get", get_proc, " path1 [path2]: Send file from server to client"},
    {"put", put_proc, " path1 [path2]: Send file from client to server"},
    {"help", help_proc, ": Display the help message"},
    {NULL, NULL}
};


#define MAXCHAR 256
#define DATASIZE 1024
#define TIMEOUT 30

struct ftpmsg {
    uint8_t type;
    uint8_t  code;
    uint16_t datalen;
    char data[0];
};

struct ftpmsg_no_data {
    uint8_t type;
    uint8_t  code;
    uint16_t datalen;
};
