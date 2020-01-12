/*#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>*/
#include "my.h"

//proc.c
extern void print_buf(struct ftpmsg);
extern void send_msg(int dstSocket, uint8_t type, uint8_t code, char  *data);

int socket_init(const char *destination);

int main(int argc, char ** argv) {

    char destination[MAXCHAR];
    memset(destination, 0, sizeof destination);
  

    if (argc == 2) {
        strncpy(destination, argv[1], sizeof(destination));
    } else {
        fprintf(stderr, "Please input the host name\n");
        exit(1);
    }

    int dstSocket = socket_init(destination);
    
    int i;
    for (i = 0; i < 2; i++) {
        printf("sending...\n");
        send_msg(dstSocket, CMDERR, 0x02, "msg");

        struct ftpmsg rmsg;
        memset(&rmsg, 0, sizeof rmsg);
        printf("received:\n");
        recv(dstSocket, &rmsg, sizeof(rmsg), 0);
        print_buf(rmsg);
        sleep(1);
    }
    send_msg(dstSocket, CMDERR, 0x02, "client finish");
    printf("client finished\n");
    close(dstSocket);
    //freeaddrinfo(res);
    return 0;
}




int socket_init(const char *destination)
{
    unsigned short port = 50012;
    fprintf(stderr, "CAUTION: I'm using port %d for the congestion avoidance\n", port);
    int dstSocket;

    struct sockaddr_in dstAddr;
    memset( &dstAddr, 0, sizeof(dstAddr));

    struct addrinfo hints, *res;
    char *node;
    node = malloc((sizeof (char)) * MAXCHAR);
    memset(node, 0, MAXCHAR);
    strncpy(node, destination, MAXCHAR);
    printf("OK. I will get IP from %s\n", node);
    //char *serv = "http";
    char *serv = NULL;
    int err, sd;

    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;
    if ((err = getaddrinfo(node, serv, &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        exit(1);
    }


    //res->ai_addrがソケットアドレスの領域を指す (struct sockaddr *)

    if ((sd = socket(res->ai_family, res->ai_socktype,
             res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }

    char address[256];
    memset(address, 0, sizeof address);
    void *ptr = &((struct sockaddr_in*)res->ai_addr)->sin_addr;
    inet_ntop(res->ai_family, ptr, address, sizeof address);
    printf("The host IP:%s\n", address);
    sprintf(destination,"%s", address);

    struct in_addr ipaddr;

    if (inet_aton(destination, &ipaddr) <= 0) {
        fprintf(stderr, "inet_aton: error\n");
        exit(1);
    }
    dstAddr.sin_addr.s_addr = ipaddr.s_addr;

    dstAddr.sin_port = htons(port);
    dstAddr.sin_family = AF_INET;

    //dstAddr.sin_addr.s_addr = inet_addr(destination);
    dstSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(dstSocket, &dstAddr, sizeof(dstAddr)) < 0) {
    //if (connect(dstSocket, (struct sockaddr * ) & dstAddr, sizeof(dstAddr)) < 0) {
        perror("connect");
        exit(1);
    }
    return dstSocket;
}





