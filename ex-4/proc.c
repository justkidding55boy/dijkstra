#include "mydhcp.h"


void print_buf(struct dhcph buf)
{
    struct in_addr in;
    in.s_addr = buf.address;
    printf("type %d(%s), " ,buf.type, mytypes[buf.type-1]);
    printf("code %d, ttl %d, IP %s, ", buf.code, buf.ttl, inet_ntoa(in));
    in.s_addr = buf.netmask;
    printf("mask %s\n", inet_ntoa(in));
    fprintf(stderr, "*****************************\n\n\n");
}


void print_client_list()
{
    printf("*****print_client_list*******\n");
    struct client *myp;
    for (myp = client_list.fp; myp != &client_list; myp  = myp->fp) {
        struct in_addr in;
        in = myp->addr;

        printf("status: %s IP %s\n",  mytypes[myp->status-1], inet_ntoa(in));
    }

    printf("*****print_client_list*******\n");
}
