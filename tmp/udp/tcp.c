#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <error.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char **argv)
{
    int s;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in skt;
    struct in_addr ipaddr;
    inet_aton("131.113.108.54", &ipaddr);

    char *buf = argv[1];

    int count, datalen = strlen(buf);
    skt.sin_family = AF_INET;
    skt.sin_port = htons(49152);

    skt.sin_addr.s_addr = ipaddr.s_addr;

    printf("%d\n", datalen);
    if ((count = sendto(s, buf, datalen, 0, (struct sockaddr *) &skt, sizeof skt)) < 0) {
        perror("sendto");

        exit(1);
    }
    printf("%d\n", count);
    return 0;
}

