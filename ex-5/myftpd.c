
#include "my.h"

#define BUFFER_SIZE 256
//proc.c
extern void print_buf(struct ftpmsg);
extern void send_msg(int dstSocket, uint8_t type, uint8_t code, char *data);
extern void time_init();
void server_execute(int dstSocket, struct ftpmsg rmsg);

int socket_init();
int listen_accept(int);

void sig_alrm()
{
    printf("time_alrt\n");
}

void sig_pipe()
{
    printf("sig_pipe\n");
}
int main() {
  
    int srcSocket = socket_init();
    int dstSocket = listen_accept(srcSocket);

    //time_init();
    signal(SIGALRM, sig_alrm);
    signal(SIGPIPE, sig_pipe);

    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(0, &rdfds);
    FD_SET(dstSocket, &rdfds);
    struct timeval tv;
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    int retval = select(dstSocket+1, &rdfds, NULL, NULL, &tv);

    /* パケット受信 */
    while (1) {

        if (retval == -1) {
            printf("retval: -1\n");
            
        } else if (retval) {
            //なんらかの更新
            printf("retval>0\n");
            struct ftpmsg rmsg;
            memset(&rmsg, 0, sizeof rmsg);

            int r;
            if ((r = recv(dstSocket, &rmsg, sizeof rmsg, 0)) < 0) {
                perror("recv");
                close(dstSocket);
                dstSocket = listen_accept(srcSocket);
                if (!FD_ISSET(dstSocket, &rdfds)) {
                    retval = select(dstSocket+1, &rdfds, NULL, NULL, &tv); 
                } else {
                    printf("same user: skip\n");
                }
                break;
            } else if  (r == 0) {
                close(dstSocket);
                printf("finish\n");
                dstSocket = listen_accept(srcSocket);
                if (!FD_ISSET(dstSocket, &rdfds)) {
                    retval = select(dstSocket+1, &rdfds, NULL, NULL, &tv); 
                } else {
                    printf("same user: skip\n");
                }
                continue;
            }
            printf("received:");
            print_buf(rmsg);
            server_execute(dstSocket, rmsg);

            } else {
                printf("retval==0\n");
            }

    }

    return 0;
}

int socket_init()
{
  /* ポート番号、ソケット */
    unsigned short port = 51199;
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

int listen_accept(int srcSocket)
{
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

    return dstSocket;

}
