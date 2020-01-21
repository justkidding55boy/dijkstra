#include "my.h"
void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data);
void print_buf(struct ftpmsg *buf);
int errcheck(int type, int code);

int myport()
{
    int port = 50332;
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
        
        if (read(r, buf, DATASIZE-1) < 0) {
            send_msg(dstSocket, UNKWNERR, 0x05, NULL);
            return ;
        }        
        send_msg(dstSocket, CMD, 0x01, NULL);

        send_msg(dstSocket, DATA, 0x01, buf);
        memset(buf, 0, DATASIZE);
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


void server_execute(int dstSocket, struct ftpmsg *rmsg)
{

    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == rmsg->type) 
            if  (pt->codenum == rmsg->code || pt->codenum == -1) 
                break;
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
    rmsg->datalen = ntohs(rmsg->datalen);
    recv(dstSocket, rmsg->data, rmsg->datalen, 0);
    //print_buf(&rmsg);

    printf("server pwd: %s\n", rmsg->data);

}

void cd_proc(int dstSocket, char *av[], int ac)
{
    if (ac == 2)
        send_msg(dstSocket, CWD, 0, av[1]);
    else {
        fprintf(stderr, "please input the path\n");
        return ;
    }
    struct ftpmsg *rmsg;
    rmsg = malloc(sizeof (struct ftpmsg));
    memset(rmsg, 0, sizeof (*rmsg));
    
    recv(dstSocket, rmsg, sizeof(*rmsg), 0);
    if (errcheck(rmsg->type, rmsg->code)) {
        return ;
    }
    //print_buf(rmsg);

/*    if (rmsg->type == FILEERR) {
        if (rmsg->code == 0x00)
            fprintf(stderr, "FILEERR file doesn't exist\n");
        if (rmsg->code == 0x01)
            fprintf(stderr, "FILEERR file permission denied\n");
    }
    if (rmsg->type == UNKWNERR) {
        fprintf(stderr, "UNKWNERR occured\n");
    }*/

}

void dir_proc(int dstSocket, char *av[], int ac)
{

    if (ac == 2)
        send_msg(dstSocket, LIST, 0, av[1]);
    else if (ac == 1){
        send_msg(dstSocket, LIST, 0, NULL);
    } else {
        fprintf(stderr, "input the path properly\n");
        return ;
    }

    struct ftpmsg *rmsg;

    rmsg = malloc(sizeof (struct ftpmsg));
    
    recv(dstSocket, rmsg, sizeof(*rmsg), 0);
    if (errcheck(rmsg->type, rmsg->code)) {
        return ;
    }
    //print_buf(rmsg);
    /*
    if (rmsg->type == FILEERR) {
        if (rmsg->code == 0x00)
            fprintf(stderr, "FILERR file doesn't exist\n");
        if (rmsg->code == 0x01)
            fprintf(stderr, "FILEERR file permission denied\n");
        return ;
    } else if (rmsg->type == UNKWNERR) {
        fprintf(stderr, "UNKWNERR occured\n");
        return ;
    }*/
       
    free(rmsg);
    rmsg = malloc(sizeof (struct ftpmsg) + DATASIZE);
    
    do {
        memset(rmsg, 0, (sizeof (*rmsg))+DATASIZE);
        recv(dstSocket, rmsg, sizeof (*rmsg), 0);
        rmsg->datalen = ntohs(rmsg->datalen);
        if (rmsg->datalen != 0) {
            recv(dstSocket, rmsg->data, rmsg->datalen, 0);
            printf("%s", rmsg->data);
        }
        print_buf(rmsg);
    } while (rmsg->type == DATA && rmsg->code == 0x01); 
 
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
    memset(serverfile, 0, DATASIZE);
    memset(clientfile, 0, DATASIZE);
    if (ac > 3 || ac == 1) {
        fprintf(stderr, "please input path properly\n");
        return ;
    } else if (ac == 3) {
        strcpy(clientfile, av[2]);
    } else {
        extract_filename(av[1], clientfile);
    }

    int w = open(clientfile, O_WRONLY|O_CREAT|O_APPEND,  0644);

    if (w < 0) {
        perror("open");
        return ;
    }

    strcpy(serverfile, av[1]);
    send_msg(dstSocket, RETR, 0, serverfile);
    struct ftpmsg *rmsg;
    rmsg = malloc(sizeof (struct ftpmsg));
    memset(rmsg, 0, sizeof (*rmsg));
    
    recv(dstSocket, rmsg, sizeof(*rmsg), 0);
    if (errcheck(rmsg->type, rmsg->code)) {
        remove(clientfile);
        free(rmsg);
        return ;
    }
    /*
    print_buf(rmsg);
    if (rmsg->type == FILEERR && rmsg->code == 0x01) {
        fprintf(stderr, "FILEERR file permission denied\n");
        return ;
    } else if (rmsg->type == FILEERR && rmsg->code == 0x00) {
        fprintf(stderr, "FIILEERR file doesn't exist\n");
        return ;
    } else if (rmsg->type == UNKWNERR) {
        fprintf(stderr, "UNKWNERR occured\n");
        return ;
    }*/


    free(rmsg);
    rmsg = malloc(sizeof(struct ftpmsg) + DATASIZE);

    do {
        memset(rmsg, 0, sizeof (*rmsg) + DATASIZE);
        recv(dstSocket, rmsg, sizeof (struct ftpmsg), 0);
        rmsg->datalen = htons(rmsg->datalen);
        if (rmsg->datalen != 0) {
            recv(dstSocket, rmsg->data, rmsg->datalen, 0);
        }
        //printf("received:%dbytes\n",t);
        //printf("%s", rmsg.data);
        int cmp = rmsg->datalen > DATASIZE ? DATASIZE:rmsg->datalen;
        if (write(w, rmsg->data, cmp) < 0) {
            perror("write");
            return ;
        }
        //print_buf(rmsg);

    } while (rmsg->type == DATA && rmsg->code == 0x01); 
    //printf("get finished\n");
    close(w);
 
}

int errcheck(int type, int code)
{
    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if (pt->typenum == CMDERR)
            break;
    }

    for (; pt->typenum != DATA; pt++) {
        if (pt->typenum == type &&  pt->codenum == code) {
            fprintf(stderr, "%s\n", pt->desc);
            return 1;
        }
    }
    return 0;
}

void put_proc(int dstSocket, char *av[], int ac)
{
    char serverfile[DATASIZE];
    char clientfile[DATASIZE];
    memset(serverfile, 0, DATASIZE);
    memset(clientfile, 0, DATASIZE);
    if (ac > 3 || ac == 1) {
        fprintf(stderr, "please input path properly\n");
        return ;
    } else if (ac == 3) {
        strncpy(serverfile, av[2], DATASIZE);
    } else {
        extract_filename(av[1], serverfile);
    }
    strncpy(clientfile, av[1], DATASIZE);

    int r = open(clientfile, O_RDONLY);
    if (r < 0) {
        perror("open");
        return ;
    } 

    send_msg(dstSocket, STOR, 0, serverfile);
    struct ftpmsg *rmsg;
    rmsg = malloc(sizeof (struct ftpmsg));
    memset(rmsg, 0, sizeof (*rmsg));
    
    recv(dstSocket, rmsg, sizeof(*rmsg), 0);
    if (errcheck(rmsg->type, rmsg->code)) {
        return ;
    }
    printf("receive:");
    print_buf(rmsg);
    //if OKでない時の分岐も考える
    /*
    if (rmsg->type == CMD && rmsg->code == 0x02) {
       //OK 
    } else if (rmsg->type == FILEERR && rmsg->code == 0x01){
        fprintf(stderr, "file permission denied\n");
        return ;
    } else {
        fprintf(stderr, "unknown code\n");
        return ;
    }*/
    
    char buf[DATASIZE];
    memset(buf, 0, DATASIZE);
    int t;
    while ((t = read(r, buf, DATASIZE-1)) >  0) {
        //printf("buf:%s\n", buf);
        send_msg(dstSocket, DATA, 0x01, buf);
        memset(buf, 0, DATASIZE);
    }
    if (t < 0) 
        perror("read");

    send_msg(dstSocket, DATA, 0x00, NULL);
    close(r);
}

void sput_proc(int dstSocket, char *filename, int datalen)
{


    //最初にきたものはfilenameが入っている
    char serverfile[DATASIZE];
    memset(serverfile, 0, DATASIZE);
    if (filename != NULL)
        memcpy(serverfile, filename, datalen);
    else
        printf("filename is null\n");
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

    struct ftpmsg *rmsg;
    rmsg = malloc(sizeof (struct ftpmsg) + DATASIZE);

    do {
        memset(rmsg, 0, sizeof (struct ftpmsg) + DATASIZE);
        recv(dstSocket, rmsg, sizeof (*rmsg), 0);
        rmsg->datalen = ntohs(rmsg->datalen);
        if (rmsg->datalen != 0) {
            recv(dstSocket, rmsg->data, rmsg->datalen, 0);
        }
                  //printf("%s", rmsg->data);
        if (write(w, rmsg->data, rmsg->datalen) < 0) {
            perror("write");
            return ;
        }
        print_buf(rmsg);
    } while (rmsg->type == DATA && rmsg->code == 0x01); 
    close(w);

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
            (*p->func)(dstSocket, argv, argc);
            return 1;
        }

    }
    printf("command not found\n");

    return 0;
}


void print_buf(struct ftpmsg *buf)
{
    struct commands *pt;
    for (pt = cmdtbl; pt->desc; pt++) {
        if  (pt->typenum == buf->type) 
            if  (pt->codenum == buf->code || pt->codenum == -1) 
                break;
    }
    
    printf("type 0x%x, " ,buf->type);
    printf("code 0x%x, (%s), datalen %d\n", buf->code, pt->desc, buf->datalen);
    /*
    if (buf->datalen != 0)
        printf("data:%s\n", buf->data);*/

    /*if (buf->code == FILEERR || buf->code  == UNKWNERR)
        printf("%s\n",  pt->desc);*/

}

void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data)
{

    struct ftpmsg *msg;
    if (data == NULL) {
        msg = (struct ftpmsg*)malloc(sizeof(struct ftpmsg));
        memset(msg, 0, sizeof(struct ftpmsg));
        msg->datalen = htons(0);
        msg->type = type;
        msg->code = code;
        int t;
        //printf("sent:");
        //printf("sizeof:%d\n", sizeof (*msg));
        
        if ((t = send(dstSocket, msg, sizeof (*msg), 0)) < 0) {
            perror("send");
            //exit(1);
        }
    } else {
        msg = (struct ftpmsg *)malloc(sizeof(struct ftpmsg) + DATASIZE);
        memset(msg, 0, sizeof (struct ftpmsg) + DATASIZE);
        memcpy(msg->data, data, strlen(data));
        msg->datalen = htons((uint16_t)strlen(data));
        msg->type = type;
        msg->code  = code;
        int t;
        //printf("sent:");
        //printf("sizeof:%d\n", sizeof (*msg) + msg->datalen);
        
        if ((t = send(dstSocket, msg, sizeof (*msg) + strlen(data), 0)) < 0) {
            perror("send");
            //exit(1);
        }
        //printf("%dbyte\n", t);
    }
    msg->datalen = ntohs(msg->datalen);
    print_buf(msg);
    free(msg);

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
