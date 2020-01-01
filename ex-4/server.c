#include <stdio.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>

int main()
{
    int s;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
    }

    return 0;
}
