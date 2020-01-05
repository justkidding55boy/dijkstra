#include <stdio.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    int s, count;
    in_port_t myport = 49152;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
    }

	struct sockaddr_in myskt; //自ソケット
	struct sockaddr_in skt; //相手ソケット
    char rbuf[512];         //受信用バッファ
    socklen_t sktlen;

	memset(&myskt, 0, sizeof myskt);
	memset(&skt, 0, sizeof skt);
	myskt.sin_family = AF_INET;
	//IPアドレスやポート番号を扱う場合にはhtonl()が必要
	myskt.sin_port = htons(myport); //ポート番号
	myskt.sin_addr.s_addr = htonl(INADDR_ANY); //IP アドレス

	if (bind(s, (struct sockaddr *)&myskt, sizeof(myskt)) < 0) {
		perror("bind");
		exit(1);
	}
    printf("hello\n");

    sktlen = sizeof skt;
    while (1) {
        memset(rbuf, 0, 512);
        if ((count = recvfrom(s, rbuf, sizeof rbuf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        printf("received:%s", rbuf);
        
        if ((count = sendto(s, rbuf, strlen(rbuf), 0, (struct sockaddr *) &skt, sizeof skt)) < 0) {
            perror("sendto");
            exit(1);
        }
        

    }

    // skt.sin_port に送信側のポート番号が設定されている 
    // skt.sin_addr.s_addr に送信側の IP アドレスが設定されている 
    // バイトオーダに気をつけること
    // sktlen に相手側ソケットアドレスの実際のサイズが返される
    return 0;
}
