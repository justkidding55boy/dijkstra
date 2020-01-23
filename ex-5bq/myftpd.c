
#include "my.h"

#define BUFFER_SIZE 256
//proc.c
extern void print_buf(struct ftpmsg*);
extern void send_msg(int dstSocket, uint8_t type, uint8_t code, char *data);
extern void time_init();
void server_execute(int dstSocket, struct ftpmsg *rmsg);

int socket_init();

void sig_alrm()
{
    printf("time_alrt\n");
}

void sig_pipe()
{
    printf("sig_pipe\n");
}

void sig_chld()
{
    int status;
    wait(&status);
}
int main() {
  
    int srcSocket = socket_init();

    //time_init();
    signal(SIGALRM, sig_alrm);
    signal(SIGPIPE, sig_pipe);
    signal(SIGCHLD, sig_chld);

    /* パケット受信 */
    while (1) {
        //int dstSocket;
        struct sockaddr_in dstAddr;
        socklen_t dstAddrSize = sizeof(dstAddr);
        memset(&dstAddr, 0, sizeof(dstAddr));
        /* 接続の許可 */
        if (listen(srcSocket, 5) < 0) {
            perror("listen");
            exit(1);
        }
           /* 接続の受付け */
        printf("Waiting for connection ...\n");
        int dstSocket; // 相手
        dstSocket = accept(srcSocket, (struct sockaddr * ) &dstAddr, &dstAddrSize);
        if (dstSocket < 0) {
            perror("accept");
            exit(1);
        }
        printf("Connected from %s\n", inet_ntoa(dstAddr.sin_addr));

        if (fork() ==  0) {
 
            struct ftpmsg *rmsg;
            rmsg = malloc(sizeof (struct ftpmsg));
            while (1) {

                memset(rmsg, 0, sizeof (struct ftpmsg));

                int r;
                if ((r = recv(dstSocket, rmsg, sizeof (struct ftpmsg), 0)) < 0) {
                    perror("recv");
                    close(dstSocket);
                    exit(0);

                    break;
                } else if  (r == 0) {
                    close(dstSocket);
                    printf("finish\n");
                    exit(0);
                }

                rmsg->datalen = ntohs(rmsg->datalen);

                print_buf(rmsg);

                struct commands *pt;
                for (pt = cmdtbl; pt->desc; pt++) {
                    if  (pt->typenum == rmsg->type)
                        if  (pt->codenum == rmsg->code || pt->codenum == -1)
                            break;
                }

                if (pt == NULL) {
                    send_msg(dstSocket, CMDERR, 0x01, NULL);
                    continue;
                }

                if (rmsg->datalen == 0)
                    (*pt->func)(dstSocket, NULL, 0);
                else if (rmsg->datalen <= DATASIZE) {
                    char buf[DATASIZE];
                    memset(buf, 0, DATASIZE);
                    (r = recv(dstSocket, buf, rmsg->datalen, 0));
                    (*pt->func)(dstSocket, buf, rmsg->datalen);

                }
                if (rmsg->type == QUIT) {
                    close(dstSocket);
                    printf("finish\n");
                    exit(0);
                }
            }

        } 


    }

    return 0;
}

extern int myport();
int socket_init()
{
  /* ポート番号、ソケット */
    unsigned short port = myport();
    int srcSocket; // 自分


    /* sockaddr_in 構造体 */
    struct sockaddr_in srcAddr;

    memset(&srcAddr, 0, sizeof(srcAddr));

    srcAddr.sin_port = htons(port);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((srcSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(srcSocket, (struct sockaddr *) &srcAddr, sizeof(srcAddr)) < 0) {
        printf("binderror\n");
        perror("bind");
        exit(1);
    }

    return srcSocket;
}


