#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <error.h>
#include <arpa/inet.h>
#include <string.h>
#define MAXCHAR 256

void discover_proc(int s, struct sockaddr_in myskt)
{
    char buf[MAXCHAR];
    memset(buf, 0, MAXCHAR);
    sprintf(buf, "DISCOVER");

    if ((sendto(s, buf, strlen(buf), 0, (struct sockaddr *) &myskt, sizeof (myskt))) < 0) {
        perror("sendto");

        exit(1);
    }

}

int main(int argc, char **argv)
{
    int s;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in myskt;
    struct sockaddr_in skt;
    memset(&myskt, 0, sizeof myskt);
    memset(&skt, 0, sizeof skt);
    socklen_t sktlen = sizeof skt;

    struct in_addr ipaddr;
    inet_aton("127.0.0.1", &ipaddr);

    int count;
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(49152);

    myskt.sin_addr.s_addr = ipaddr.s_addr;


    discover_proc(s, myskt);
    

    char rbuf[512];
    memset(rbuf, 0, sizeof rbuf);

    
    if ((count = recvfrom(s, rbuf, sizeof rbuf, 0, (struct sockaddr * ) &skt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    printf("received:%s\n", rbuf);

    
    return 0;
}

