#include <stdio.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    int s;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
    }

	struct sockaddr_in skt;
	memset(&skt, 0, sizeof skt);

	memset(skt.sin_zero, 0, 8);
	skt.sa_family_t = AF_INET;
	//IPアドレスやポート番号を扱う場合にはhtonl()が必要
	skt.sin_port = 0; //ポート番号
	skt.sin_addr.a_addr = 0; //IP アドレス

	if (bind(s, (struct sockaddr *)&skt, sizeof(skt)) < 0) {
		perror("bind");
		exit(1);
	}
    return 0;
}
