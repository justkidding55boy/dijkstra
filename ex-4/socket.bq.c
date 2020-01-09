/*#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <error.h>
#include <arpa/inet.h>
#include <string.h>*/
#include "mydhcp.h"
int s;
struct sockaddr_in myskt;
struct sockaddr_in skt;
uint16_t ttl;
in_addr_t address;
in_addr_t netmask;

//proc.c
extern void print_buf(struct dhcph);

void sdiscover()
{
   struct dhcph buf;
   memset(&buf, 0, sizeof buf);
   buf.type = SDISCOVER;
   fprintf(stderr, "send\n");
   print_buf(buf);

    if ((sendto(s, &buf, sizeof buf, 0, (struct sockaddr *) &myskt, sizeof (myskt))) < 0) {
        perror("sendto");
        exit(1);
    }

}

void srequest()
{
   struct dhcph buf;
   memset(&buf, 0, sizeof buf);
   buf.type = SREQUEST;
   buf.code = 2; //割り当て要求
   //buf.code = 3; //使用期間延長要求
   buf.ttl = ttl;

   fprintf(stderr, "send\n");
   print_buf(buf);

    if ((sendto(s, &buf, sizeof buf, 0, (struct sockaddr *) &myskt, sizeof (myskt))) < 0) {
        perror("sendto");
        exit(1);
    }

}

void srequest2()
{
   struct dhcph buf;
   memset(&buf, 0, sizeof buf);
   buf.type = SREQUEST;
   //buf.code = 2; //割り当て要求
   buf.code = 3; //使用期間延長要求
   buf.ttl = ttl;
   buf.address = address;
   buf.netmask = netmask;

   fprintf(stderr, "send\n");
   print_buf(buf);

    if ((sendto(s, &buf, sizeof buf, 0, (struct sockaddr *) &myskt, sizeof (myskt))) < 0) {
        perror("sendto");
        exit(1);
    }

}
int main(int argc, char **argv)
{
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&myskt, 0, sizeof myskt);
    memset(&skt, 0, sizeof skt);
    socklen_t sktlen = sizeof skt;

    struct in_addr ipaddr;
    inet_aton("127.0.0.1", &ipaddr);

    int count;
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(51230);

    myskt.sin_addr.s_addr = ipaddr.s_addr;

    sdiscover();
    

    struct dhcph rbuf;
    memset(&rbuf, 0, sizeof rbuf);

    
    if ((count = recvfrom(s, &rbuf, sizeof rbuf, 0, (struct sockaddr * ) &skt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    printf("received:\n");
    print_buf(rbuf);
    ttl = rbuf.ttl; 

    srequest();

    if ((count = recvfrom(s, &rbuf, sizeof rbuf, 0, (struct sockaddr * ) &skt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    printf("received:\n");
    print_buf(rbuf);

    address = rbuf.address;
    netmask = rbuf.netmask;

    srequest2();

    if ((count = recvfrom(s, &rbuf, sizeof rbuf, 0, (struct sockaddr * ) &skt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    printf("received:\n");
    print_buf(rbuf);
 
    return 0;
}

