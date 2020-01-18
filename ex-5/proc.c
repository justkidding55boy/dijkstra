#include "my.h"

void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data);

void spwd_proc(int dstSocket) {
    printf("server pwd\n");
    char path[MAXCHAR];
    if (getcwd(path, sizeof path) == NULL) {
        perror("getcwd");
    }
    send_msg(dstSocket, CMD, 0x00, path);

}

void server_execute(int dstSocket, struct ftpmsg rmsg)
{
    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == rmsg.type) 
            if  (pt->codenum == rmsg.code || pt->codenum == -1) 
                break;
    }

    if (rmsg.datalen <= MAXCHAR)
        (*pt->func)(dstSocket, rmsg.data);

    
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
    printf("received:\n");
    recv(dstSocket, &rmsg, sizeof(rmsg), 0);
    print_buf(rmsg);
    printf("server pwd: %s\n", rmsg.data);

}

void cd_proc(int dstSocket, char *av[], int ac)
{
    if (ac == 2)
        send_msg(dstSocket, CWD, -1, av[1]);
    else 
        fprintf(stderr, "please input the path\n");

}

void dir_proc(int dstSocket, char *av[], int ac)
{

    if (ac == 2)
        send_msg(dstSocket, LIST, -1, av[1]);
    else if (ac == 1){
        send_msg(dstSocket, LIST, -1, NULL);
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

int execute(int dstSocket, char *argv[], int argc)
{

    struct cmds *p;

    for (p = cmdtab; p->cmd; p++) {

        if (strcmp(argv[0], p->cmd) == 0) {
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


    fprintf(stderr, "*****************************\n\n\n");
}

void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data)
{
        struct ftpmsg msg;
        memset(&msg, 0, sizeof msg);
        msg.type = type;
        msg.code  = code;
        sprintf(msg.data, data);
        msg.datalen = strlen(msg.data);
        if (send(dstSocket, &msg, sizeof msg, 0) < 0) {
            perror("send");
            //exit(1);
        }
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
