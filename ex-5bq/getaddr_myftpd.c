#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include "my.h"

#define BUFFER_SIZE 256

int main() {


    ////////////////////getaddrinfo
    
    struct addrinfo hints, *res;
    struct sockaddr_storage sin;
    //struct sockarr_storage sin;
    char *serv;
    int sd, sd1, err, sktlen;

    serv = "http";
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    if ((err = getaddrinfo(NULL, serv, &hints, &res)) < 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    }

    if ((sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        exit(1);
    }

    freeaddrinfo(res);

    if (listen(sd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    sktlen = sizeof(struct sockaddr_storage);
    if ((sd1 = accept(sd, &sin, &sktlen)) < 0) {
        perror("accept");
        exit(1);
    }


    int status;
    while (1) {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof buffer);
        int numrcv;
        numrcv = recv(sd1, buffer, BUFFER_SIZE, 0);
        if (numrcv == 0 || numrcv == -1) {
            status = close(sd1);
            break;
        }
        printf("received: %s\n", buffer);
        char sbuffer[BUFFER_SIZE];
        memset(sbuffer, 0, sizeof sbuffer);
        strncpy(sbuffer, buffer, sizeof(sbuffer));
        send(sd1, sbuffer, strlen(sbuffer), 0);
    }



    ////////////////////getaddrinfo

    /*
    unsigned short port = 50012;
    int srcSocket; // 自分
    int dstSocket; // 相手

    struct sockaddr_in srcAddr;
    struct sockaddr_in dstAddr;
    socklen_t dstAddrSize = sizeof(dstAddr);

    int numrcv;
    char buffer[BUFFER_SIZE];

    memset( & srcAddr, 0, sizeof(srcAddr));
    memset( & dstAddr, 0, sizeof(dstAddr));
    srcAddr.sin_port = htons(port);
    srcAddr.sin_family = AF_INET;
    srcAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((srcSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    if (bind(srcSocket, (struct sockaddr * ) & srcAddr, sizeof(srcAddr)) < 0) {
        printf("binderror\n");
        perror("bind");
        exit(1);
    }

    if (listen(srcSocket, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Waiting for connection ...\n");
    dstSocket = accept(srcSocket, (struct sockaddr * ) & dstAddr, & dstAddrSize);
    if (dstSocket < 0) {
        perror("accept");
        exit(1);
    }
    printf("Connected from %s\n", inet_ntoa(dstAddr.sin_addr));
    

    int status;
    while (1) {
        memset(buffer, 0, sizeof buffer);
        numrcv = recv(dstSocket, buffer, BUFFER_SIZE, 0);
        if (numrcv == 0 || numrcv == -1) {
            status = close(dstSocket);
            break;
        }
        printf("received: %s\n", buffer);
        char sbuffer[BUFFER_SIZE];
        memset(sbuffer, 0, sizeof sbuffer);
        strncpy(sbuffer, buffer, sizeof(sbuffer));
        send(dstSocket, sbuffer, strlen(sbuffer), 0);
    }
    */


    return 0;
}
