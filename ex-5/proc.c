#include "my.h"

void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data);
void print_buf(struct ftpmsg buf);

int myport()
{
    return 51290;
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
        for (i = 0; i < 3; i++) malloc(sizeof(char) * DATASIZE); 

        strcpy(av[0], "ls");
        strcpy(av[1], path);

        ls_send(dstSocket, ac, av);
    }
}

void sget_proc(int dstSocket, char *filename, int datalen)
{
    int r = open(filename, O_RDONLY);
    if (r < 0) {
        if (errno == EACCES) {
            send_msg(dstSocket, FILEERR, 0x01, NULL);
            return ;
        } else {
            send_msg(dstSocket, UNKWNERR, 0x05, NULL);
        }
    } else {
        send_msg(dstSocket, CMD, 0x01, NULL);
    }

    char buf[DATASIZE];
    memset(buf, 0, DATASIZE);
    int t;
    while ((t = read(r, buf, DATASIZE-1)) >  0) {
        send_msg(dstSocket, DATA, 0x01, buf);
        memset(buf, 0, DATASIZE);
    }
    if (t < 0) 
        perror("read");

    send_msg(dstSocket, DATA, 0x00, NULL);
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
    print_buf(rmsg);
    //printf("%s", rmsg.data);
    do {
        memset(&rmsg, 0, sizeof rmsg);
        recv(dstSocket, &rmsg, sizeof rmsg, 0);
        printf("%s", rmsg.data);
        //print_buf(rmsg);
    } while (rmsg.type == DATA && rmsg.code == 0x01); 
 
}

void extract_filename(char *path, char *target)
{
    char *ans;
    ans = strrchr(path, '/');
    if (ans == NULL)
        strcpy(target, path);
    else {
        strcpy(target, ans+1);
    }
}

void get_proc(int dstSocket, char *av[], int ac)
{
    char serverfile[DATASIZE];
    char clientfile[DATASIZE];
    if (ac > 3 || ac == 1) {
        fprintf(stderr, "please input path properly\n");
        return ;
    } else if (ac == 3) {
        strcpy(clientfile, av[2]);
    } else {
        extract_filename(av[1], clientfile);
    }
    strcpy(serverfile, av[1]);
    send_msg(dstSocket, RETR, 0, serverfile);
    struct ftpmsg rmsg;
    memset(&rmsg, 0, sizeof rmsg);
    printf("received:");
    recv(dstSocket, &rmsg, sizeof(rmsg), 0);
    print_buf(rmsg);

    int w = open(clientfile, O_WRONLY|O_CREAT|O_APPEND,  0644);

    if (w < 0) {
        perror("open");
        return ;
    }
    if (write(w, rmsg.data, rmsg.datalen) < 0) {
        perror("write");
        return ;
    }


    do {
        memset(&rmsg, 0, sizeof rmsg);
        recv(dstSocket, &rmsg, sizeof rmsg, 0);
        printf("%s", rmsg.data);
        if (write(w, rmsg.data, rmsg.datalen) < 0) {
            perror("write");
            return ;
        }
        //print_buf(rmsg);
    } while (rmsg.type == DATA && rmsg.code == 0x01); 
 
}

void put_proc(int dstSocket, char *av[], int ac)
{
    char serverfile[DATASIZE];
    char clientfile[DATASIZE];
    if (ac > 3 || ac == 1) {
        fprintf(stderr, "please input path properly\n");
        return ;
    } else if (ac == 3) {
        strcpy(serverfile, av[2]);
    } else {
        extract_filename(av[1], serverfile);
    }
    strcpy(clientfile, av[1]);

    int r = open(clientfile, O_RDONLY);
    if (r < 0) {
        if (errno == EACCES) {
            fprintf(stderr, "permission denied\n");
        } else {
            fprintf(stderr, "ukknown error\n");
        }
        return ;
    } 


    send_msg(dstSocket, STOR, 0, serverfile);
    struct ftpmsg rmsg;
    memset(&rmsg, 0, sizeof rmsg);
    printf("received:");
    recv(dstSocket, &rmsg, sizeof(rmsg), 0);
    print_buf(rmsg);
    //if OKでない時の分岐も考える
    if (rmsg.type == CMD && rmsg.code == 0x02) {
       //OK 
    } else if (rmsg.type == FILEERR && rmsg.code == 0x01){
        fprintf(stderr, "file permission denied\n");
        return ;
    } else {
        fprintf(stderr, "unknown code\n");
        return ;
    }
    
    char buf[DATASIZE];
    memset(buf, 0, DATASIZE);
    int t;
    while ((t = read(r, buf, DATASIZE-1)) >  0) {
        send_msg(dstSocket, DATA, 0x01, buf);
        memset(buf, 0, DATASIZE);
    }
    if (t < 0) 
        perror("read");

    send_msg(dstSocket, DATA, 0x00, NULL);
}

void sput_proc(int dstSocket, char *filename, int datalen)
{
    struct ftpmsg rmsg;

    //最初にきたものはfilenameが入っている
    char serverfile[DATASIZE];
    memcpy(serverfile, filename, datalen);
    //serverfile[datalen] = '\0';

   int w = open(serverfile, O_WRONLY|O_CREAT|O_APPEND,  0644);

    if (w < 0) {
        if (errno == EACCES) {
            send_msg(dstSocket, FILEERR, 0x01, NULL);
        } else {
            send_msg(dstSocket, UNKWNERR, 0x05, NULL);
        }
        return ;
    } else
        send_msg(dstSocket, CMD,  0x02, NULL);


    do {
        memset(&rmsg, 0, sizeof rmsg);
        recv(dstSocket, &rmsg, sizeof rmsg, 0);
        printf("%s", rmsg.data);
        if (write(w, rmsg.data, rmsg.datalen) < 0) {
            perror("write");
            return ;
        }
        //print_buf(rmsg);
    } while (rmsg.type == DATA && rmsg.code == 0x01); 

}



void lpwd_proc()
{
    char path[MAXCHAR];
    if (getcwd(path, sizeof path) == NULL) {
        perror("getcwd");
    }
    printf("lpwd:%s\n", path);
}

void lcd_proc(int dstSocket, char *av[], int ac)
{
    char path[MAXCHAR];
    memset(path, 0, sizeof path);

    if (ac != 2) {
        fprintf(stderr, "input path\n");
        return ;
    }

    memcpy(path, av[1], strlen(av[1])+1);


    if (path == NULL) {
        chdir(getenv("HOME"));
    } else {
        if (strcmp(path, "~") == 0) {
            chdir(getenv("HOME"));
        } else {
            if (chdir(path) < 0) {
                fprintf(stderr, "file error\n");
            }
        }
    }
}

extern void ls(int argc, char *argv[]);
void ldir_proc(int dstSocket, char *av[], int ac)
{

    ls(ac, av);
}


void help_proc()
{

    printf("Display the help\n");
    struct cmds *p;

    for (p = cmdtab; p->cmd; p++) {

        printf("%5s%s\n", p->cmd, p->desc);

    }
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
