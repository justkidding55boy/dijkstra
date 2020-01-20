#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {

    //char * hostname = "http://logex00.educ.cc.keio.ac.jp/";
    char *node = "logex00.educ.cc.keio.ac.jp";
    char *serv = NULL;
    //char *serv = "http";
    struct addrinfo * res, * res0;
    struct addrinfo hints;

    memset( &hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; //  AF_UNSPECとすることで、AF_INET AF6_INETの２つを定義できる。
    //hints.ai_family = AF_UNSPEC; //  AF_UNSPECとすることで、AF_INET AF6_INETの２つを定義できる。
    hints.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(node, serv, & hints, & res) < 0) {
        printf("ERROR! LINE:%c", __LINE__);
        exit(1);
    }
    char addr_buf[64];
    res0 = res;
    int i = 0;
    void * ptr;
    for (; res; res = res->ai_next) {
        printf("STRUCT: %d\n", i);
        printf("FAMILY: %d\n", res->ai_family);
        if (res->ai_family == AF_INET) {
            ptr = & ((struct sockaddr_in * ) res->ai_addr)->sin_addr;
        } else if (res->ai_family == AF_INET6) {
            ptr = & ((struct sockaddr_in6 * ) res->ai_addr)->sin6_addr;
        }
        inet_ntop(res->ai_family, ptr, addr_buf, sizeof(addr_buf));
        printf("ADDRESS: %s\n", addr_buf);
        i++;
    }
    printf("END\n");
    freeaddrinfo(res0);

    return 0;
}
