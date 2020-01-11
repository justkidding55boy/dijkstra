#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>



int main(int argc, char ** argv) {

    char destination[80];
    memset(destination, 0, sizeof destination);
    unsigned short port = 50012;
    int dstSocket;

    struct sockaddr_in dstAddr;
    memset( & dstAddr, 0, sizeof(dstAddr));
    char *toSendText = "This is a test";

    if (argc == 2) {
        strncpy(destination, argv[1], strlen(destination));
    } else {
        sprintf(destination, "131.113.108.53");
        //sprintf(destination, "127.0.0.1");
    }

    //////////////////////DNS///
    
    struct addrinfo hints, *res;
    char *node = "logex00.educ.cc.keio.ac.jp";
    //char *serv = "http";
    char *serv = NULL;
    int err, sd;

    memset(&hints, 0, sizeof hints);
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
    printf("sd:%d\n", sd);
    char address[256];
    memset(address, 0, sizeof address);
    void *ptr = &((struct sockaddr_in*)res->ai_addr)->sin_addr;
    inet_ntop(res->ai_family, ptr, address, sizeof address);
    printf("IP:%s\n", address);
    printf("len:%d\n", res->ai_addrlen);
    sprintf(destination,"%s", address);

    /*
    if (connect(sd, res->ai_addr, res->ai_addrlen) < 0) {
    //if (connect(sd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        exit(1);
    }
    */
    


    //////////////////////DNS///

    
    
    struct in_addr ipaddr;
    //memset( &ipaddr, 0, sizeof ipaddr);
    printf("destination:%s\n", destination);
    if (inet_aton(destination, &ipaddr) <= 0) {
        fprintf(stderr, "inet_aton: error\n");
        exit(1);
    }
    dstAddr.sin_addr.s_addr = ipaddr.s_addr;

    dstAddr.sin_port = htons(port);
    dstAddr.sin_family = AF_INET;

    //dstAddr.sin_addr.s_addr = inet_addr(destination);
    dstSocket = socket(AF_INET, SOCK_STREAM, 0);

    printf("Trying to connect to %s: \n", destination);
    if (connect(dstSocket, &dstAddr, sizeof(dstAddr)) < 0) {
    //if (connect(dstSocket, (struct sockaddr * ) & dstAddr, sizeof(dstAddr)) < 0) {
        perror("connect");
        exit(1);
    }
    
    

    int i;
    for (i = 0; i < 10; i++) {
        printf("sending...\n");
        //send(sd, toSendText, strlen(toSendText) + 1, 0);
        send(dstSocket, toSendText, strlen(toSendText) + 1, 0);
        char buffer[256];
        memset(buffer, 0, sizeof buffer);
        recv(dstSocket, buffer, sizeof(buffer), 0);
        //recv(sd, buffer, sizeof(buffer), 0);
        printf("receive:%s\n", buffer);
        sleep(1);
    }

    //close(dstSocket);
    freeaddrinfo(res);
    return 0;
}

