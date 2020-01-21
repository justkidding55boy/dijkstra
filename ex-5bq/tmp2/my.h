#include "my.h"
void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data);
void print_buf(struct ftpmsg *buf);
int errcheck(int type, int code);

int myport()
{
    int port = 50021;
    printf("port:%d\n", port);
    return port;
}

void spwd_proc(int dstSocket) {

    char path[MAXCHAR];
    if (getcwd(path, sizeof path) == NULL) {
        perror("getcwd");
    }
    printf("mypath:%s\n", path);
    send_msg(dstSocket, CMD, 0x00, path);

}

void scd_proc(int dstSocket, char *buf, int buflen) 
{
    char path[DATASIZE];
    memset(path, 0, sizeof path);

    memcpy(path, buf, buflen);
    path[buflen] = '\0';

    if (path == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (strcmp(path, "~") == 0) {
            chdir(getenv("HOME"));
            send_msg(dstSocket, CMD, 0x00, NULL);
        } else {
            if (chdir(path) < 0) {
                if (errno == EACCES) {
                    send_msg(dstSocket, FILEERR, 0x01, NULL);
                } else if (errno == ENOENT || errno == ENOTDIR) {
                    send_msg(dstSocket, FILEERR, 0x00, NULL);
                }
            } else {
                send_msg(dstSocket, CMD, 0x00, NULL);
                printf("pwd changed:%s\n", path);
            }
        }
    }
}

void squit_proc(int dstSocket, char *filename, int datalen)
{
    send_msg(dstSocket, CMD, 0x00, NULL);
}

extern void ls_send(int dstSocket, int argc, char *argv[]);
void slist_proc(int dstSocket, char *path, int pathlen)
{
    if (path == NULL) {
        int ac = 1;
        char *av[] = {"ls", NULL};
        ls_send(dstSocket, ac, av);
    } else {
        int ac = 2;
        char **av;
        av = malloc(sizeof(char *) * 3);
        int i;
        for (i = 0; i < 3; i++) av[i] = malloc(sizeof(char) * DATASIZE); 

        strcpy(av[0], "ls");
        strcpy(av[1], path);

        ls_send(dstSocket, ac, av);
    }
}

void sget_proc(int dstSocket, char *filename, int datalen)
{
    int r = open(filename, O_RDONLY);
    char buf[DATASIZE];
    memset(buf, 0, DATASIZE);
    if (r < 0) {
        if (errno == EACCES) {
            send_msg(dstSocket, FILEERR, 0x01, NULL);
        } else if (errno == 2) {
            send_msg(dstSocket, FILEERR, 0x00, NULL);
        } else {
            send_msg(dstSocket, UNKWNERR, 0x05, NULL);
        }
        return ;
    } else {
        /*
        if (read(r, buf, DATASIZE-1) < 0) {
            send_msg(dstSocket, UNKWNERR, 0x05, NULL);
            return ;
        }

        lseek(r, -1, SEEK_CUR);
        */
        send_msg(dstSocket, CMD, 0x01, NULL);
    }

    int t;
    while ((t = read(r, buf, DATASIZE-1)) >  0) {
        send_msg(dstSocket, DATA, 0x01, buf);
        memset(buf, 0, DATASIZE);
    }
    if (t < 0) 
        perror("read");

    send_msg(dstSocket, DATA, 0x00, NULL);
}


void server_execute(int dstSocket, struct ftpmsg *r                                                                                                                                                                                         
{

    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == rmsg->type) 
            if  (pt->codenum == rmsg->code || pt->codenum == -1) 
      �           break;
    }

    if (rmsg->datalen == 0)
        (*pt->func)(dstSocket, NULL, 0);
    else if (rmsg->datalen <= DATASIZE) {

        (*pt->func)(dstSocket, rmsg->data, rmsg->datalen);
        
    }
   
}

void quit_proc(int dstSocket)
{
    send_msg(dstSocket, QUIT, 0, NULL);

    struct ftpmsg *rmsg;

    rmsg = malloc(sizeof (struct ftpmsg));
    memset(rmsg, 0, sizeof *rmsg); 
    recv(dstSocket, rmsg, sizeof(*rmsg), 0);
    if (errcheck(rmsg->type, rmsg->code)) {
        return ;
    }
    exit(1);
}

void pwd_proc(int dstSocket)
{

    send_msg(dstSocket, PWD, 0, NULL);

    struct ftpmsg *rmsg;
    rmsg = malloc(sizeof (struct ftpmsg) + DATASIZE);
    memset(rmsg, 0, (sizeof *rmsg) + DATASIZE);
    
    recv(dstSocket, rmsg, sizeof(*rmsg), 0);
    if (errcheck(rmsg->type, rmsg->code)) {
        return ;
    }