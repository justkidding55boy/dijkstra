
#include "my.h"

#define BUFFER_SIZE 256
//proc.c
extern void print_buf(struct ftpmsg);
extern void send_msg(int dstSocket, uint8_t type, uint8_t code, char *data);

int socket_init();
int listen_accept(int);

int main() {
  
    int srcSocket = socket_init();
    int dstSocket = listen_accept(srcSocket);

    /* パケット受信 */
    while (1) {

        struct ftpmsg rmsg;
        memset(&rmsg, 0, sizeof rmsg);

        int r;
        if ((r = recv(dstSocket, &rmsg, sizeof rmsg, 0)) < 0) {
            perror("recv");
            close(dstSocket);
            break;
        } else if  (r == 0) {
            close(dstSocket);
            printf("finish\n");
            dstSocket = listen_accept(srcSocket);
            continue;
        }
        printf("received\n");
        print_buf(rmsg);

        printf("sending...\n");
        send_msg(dstSocket, QUIT, 0, "server to client");
    }

    return 0;
}

int socket_init()
{
  /* ポート番号、ソケット */
    unsigned short port = 50012;
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
