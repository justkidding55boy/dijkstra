#include "my.h"

void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data);
void print_buf(struct ftpmsg buf);

int myport()
{
    return 51297;
}

void spwd_proc(int dstSocket) {

    char path[MAXCHAR];
    if (getcwd(path, sizeof path) == NULL) {
        perror("getcwd");
    }
    send_msg(dstSocket, CMD, 0x00, path);

}

void scd_proc(int dstSocket, char *buf, int buflen) 
{
    char path[MAXCHAR];
    memset(path, 0, sizeof path);

    memcpy(path, buf, buflen);
    path[buflen] = '\0';


    if (path == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (strcmp(path, "~") == 0) {
            chdir(getenv("HOME"));
        } else {
            if (chdir(path) < 0) {
                send_msg(dstSocket, FILEERR, 0x00, NULL);
            } else {
                send_msg(dstSocket, CMD, 0x00, NULL);
            }
        }
    }
}

extern void ls(int dstSocket, int argc, char *argv[]);
void slist_proc(int dstSocket, char *path, int pathlen)
{
    printf("slist\n");

    if (path == NULL) {
        int ac = 1;
        char *av[] = {"ls", NULL};
        ls(dstSocket, ac, av);
    } else {
        int ac = 2;
        char **av;
        av = malloc(sizeof(char *) * 3);
        int i;
        for (i = 0; i < 3; i++) malloc(sizeof(char) * DATASIZE); 

        strcpy(av[0], "ls");
        strcpy(av[1], path);

        ls(dstSocket, ac, av);
    }
}

void server_execute(int dstSocket, struct ftpmsg rmsg)
{
    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == rmsg.type) 
            if  (pt->codenum == rmsg.code || pt->codenum == -1) 
                break;
    }
    if (rmsg.datalen == 0)
        (*pt->func)(dstSocket, NULL, 0);
    else if (rmsg.datalen <= MAXCHAR)
        (*pt->func)(dstSocket, rmsg.data, rmsg.datalen);

}

void quit_proc(int dstSocket)
{
    send_msg(dstSocket, QUIT, -1, NULL);
    //exit(1);
}

void pwd_proc(int dstSocket)
{
    send_msg(dstSocket, PWD, -1, NULL);


    struct ftpmsg rmsg;
    memset(&rmsg, 0, sizeof rmsg);
    printf("received:");
    recv(dstSocket, &rmsg, sizeof(rmsg), 0);
    print_buf(rmsg);
    printf("server pwd: %s\n", rmsg.data);

}

void cd_proc(int dstSocket, char *av[], int ac)
{
    if (ac == 2)
        send_msg(dstSocket, CWD, -1, av[1]);
    else {
        fprintf(stderr, "please input the path\n");
        return ;
    }
    struct ftpmsg rmsg;
    memset(&rmsg, 0, sizeof rmsg);
    printf("received:");
    recv(dstSocket, &rmsg, sizeof(rmsg), 0);
    print_buf(rmsg);

}

void dir_proc(int dstSocket, char *av[], int ac)
{

    if (ac == 2)
        send_msg(dstSocket, LIST, -1, av[1]);
    else if (ac == 1){
        send_msg(dstSocket, LIST, -1, NULL);
    }
    struct ftpmsg rmsg;
    memset(&rmsg, 0, sizeof rmsg);
    printf("received:");
    recv(dstSocket, &rmsg, sizeof(rmsg), 0);
    //print_buf(rmsg);
    printf("%s", rmsg.data);
    while (rmsg.code == 0x01) {
        memset(&rmsg, 0, sizeof rmsg);
        recv(dstSocket, &rmsg, sizeof rmsg, 0);
        printf("%s", rmsg.data);
        //print_buf(rmsg);
    }
 
}

void lpwd_proc()
{

}

void lcd_proc()
{

}

void ldir_proc()
{

}

void get_proc()
{

}

void put_proc()
{

}

void help_proc()
{

}

int execute(int dstSocket, char *argv[MAXCHAR], int argc)
{

    struct cmds *p;

    for (p = cmdtab; p->cmd; p++) {

        if (strcmp(argv[0], p->cmd) == 0) {
            printf("execute:%s\n", p->cmd);
            (*p->func)(dstSocket, argv, argc);
            return 1;
        }

    }
    printf("command not found\n");

    return 0;
    

}


void print_buf(struct ftpmsg buf)
{
    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == buf.type) 
            if  (pt->codenum == buf.code || pt->codenum == -1) 
                break;
    }
    printf("type 0x%x, " ,buf.type);
    printf("code 0x%x, (%s), datalen %d\n", buf.code, pt->desc, buf.datalen);
    printf("data:%s\n", buf.data);


}

void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data)
{
        struct ftpmsg msg;
        memset(&msg, 0, sizeof msg);
        msg.type = type;
        msg.code  = code;

        if (data != NULL) {
           strncpy(msg.data, data, strlen(data));
           msg.datalen = strlen(data);
        } else {
            msg.datalen = 0;
        }

        
        if (send(dstSocket, &msg, sizeof msg, 0) < 0) {
            perror("send");
            //exit(1);
        }
        printf("sent:");
        print_buf(msg);

}

void time_init()
{
    struct itimerval ttltimer;
    ttltimer.it_value.tv_sec = 1;
    ttltimer.it_value.tv_usec = 0;
    ttltimer.it_interval.tv_sec = 1;
    ttltimer.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &ttltimer, NULL) < 0) {
        perror("setitimer");
        exit(1);
    }
    printf("time_init\n");

}
