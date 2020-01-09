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
struct dhcph rbuf;
socklen_t sktlen = sizeof skt;

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
void receive_print()
{
    memset(&rbuf, 0, sizeof rbuf);

    int count;
    if ((count = recvfrom(s, &rbuf, sizeof rbuf, 0, (struct sockaddr * ) &skt, &sktlen)) < 0) {
        perror("recvfrom");
        exit(1);
    }
    printf("received:\n");
    print_buf(rbuf);

}
int main(int argc, char **argv)
{
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    memset(&myskt, 0, sizeof myskt);
    memset(&skt, 0, sizeof skt);


    struct in_addr ipaddr;
    inet_aton("127.0.0.1", &ipaddr);

    
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(51231);

    myskt.sin_addr.s_addr = ipaddr.s_addr;

    int input;
    while(1) {
        char *menu[256] = {
            "sdiscover",
            "srequest",
            "srequest2",
            NULL
        };
        int i;
        for (i = 0; i < 4; i++) {
            printf("%s: %d\n", menu[i], i);
        }
        scanf("%d", &input);
        switch (input) {
            case 0:
                sdiscover();
                receive_print();
                ttl = rbuf.ttl; 
                break;
            case 1:
                srequest();

                receive_print();
                address = rbuf.address;
                netmask = rbuf.netmask;
                break;
            case 2:
                srequest2();
                receive_print();
                break;   
            default:
                break;
        };
    }
   
    return 0;
}

