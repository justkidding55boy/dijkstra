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
struct sockaddr_in myskt;//相手
struct sockaddr_in skt; //自分
uint16_t ttl;
uint16_t rttl = 0;
in_addr_t address;
in_addr_t netmask;
struct dhcph rbuf;
socklen_t sktlen = sizeof skt;
uint8_t timeoutflag = 0;

int event;
int status;
int count;
struct client *p;
struct timeval tv;



//proc.c
extern void print_buf(struct dhcph);

void srequest(), setip();
void f_act1()
{

}

void time_init()
{
    struct itimerval ttltimer;
    ttltimer.it_value.tv_sec = 1;
    ttltimer.it_value.tv_usec = 0;
    ttltimer.it_interval.tv_sec = 1;
    ttltimer.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &ttltimer, NULL) < 0) {
        perror("settimer");
        exit(1);
    }
}

struct proctable {
    int status;
    int event;
    void (*func)();
} ptab [] = {
    {OFFER_WAIT, ROFFER, srequest},
    {ACK_WAIT, RACK, setip},
    {IPGET, TIMEHALF, f_act1},
    {IPGET, RELEASEPREP, f_act1},
    {0, 0, NULL}
};

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

    printf("state change: stat(%s) -> stat(%s)\n", mytypes[status-1], mytypes[ACK_WAIT-1]);
    status = ACK_WAIT;

}

void setip()
{
    address = rbuf.address;
    netmask = rbuf.netmask;
    printf("state change: stat(%s) -> stat(%s)\n", mytypes[status-1], mytypes[IPGET-1]);
    status = IPGET;
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
/*
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
*/
void sock_init(char *ip)
{

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }


    memset(&myskt, 0, sizeof myskt);
    memset(&skt, 0, sizeof skt);


    struct in_addr ipaddr;
    inet_aton(ip, &ipaddr);
    //inet_aton("127.0.0.1", &ipaddr);

    
    myskt.sin_family = AF_INET;
    myskt.sin_port = htons(51230);

    myskt.sin_addr.s_addr = ipaddr.s_addr;

}
uint8_t alrmflag = 0;
uint8_t hupflag = 0;

void  sigh_alrm()
{
    alrmflag++;
}

void sigh_hup()
{
    hupflag++;
}
fd_set rdfds;

int wait_event()
{
    uint8_t loop;
    int retval;
    timeoutflag = 0;
    signal(SIGALRM, sigh_alrm);
    signal(SIGHUP, sigh_hup);

    do {
        loop =  0;
        FD_ZERO(&rdfds);    FD_SET(s, &rdfds);
        tv.tv_sec = TIMEOUT;    tv.tv_usec = 0;
        errno = 0;
        retval = select(s+1, &rdfds, NULL, NULL, &tv);
        printf("retval:%d\n", retval);
        printf("status:%s\n", mytypes[status-1]);


        if (retval == -1) {
            //更新無し
       
            if (status == IPGET) {
                rttl++;
                printf("passed time: %ds\n", rttl);
                printf("ttl: %d\n", ttl);
                if (rttl >= ttl) {
                    errno = 4;
                    rttl = 0;
                }
            }


            if (errno == 4) {
                if (status == RACK && alrmflag > 0) {
                    alrmflag = 0;
                    return TIMEHALF;
                }
                loop = 1;
                continue;
            } else {
                perror("select");
                exit(1);
            }
        } else if (retval) {
           if (FD_ISSET(s, &rdfds)) {
               if ((count = recvfrom(s, &rbuf, sizeof rbuf,
                               0, (struct sockaddr*)&skt, &sktlen) < 0)) {
                       
                    perror("recvfrom");
                   exit(1); 
               }
           } 
           printf("receive\n");
           print_buf(rbuf);
           
           if (status == OFFER_WAIT && rbuf.type == SOFFER) { 
                ttl = rbuf.ttl; 
                return ROFFER;
           }    
           if (status == ACK_WAIT && rbuf.type == SACK) {
                return RACK;
           }
           fprintf(stderr, "illegal message\n");
           exit(1);
        } else {
            printf("timeout: status(%s)\n", mytypes[status-1]);
            if (status == IPGET) {
                loop = 1;
                continue;
            }
            fprintf(stderr, "timeout\n");
            if (timeoutflag) exit(0);
            else {
                timeoutflag++;
                if (status == OFFER_WAIT) return 0 ;
                if (status == RACK) return RACK;
                if (status == IPGET) return RELEASEPREP;
                loop = 1;
                continue;
            }
        }

    } while(loop);
}
int main(int argc, char **argv)
{
    if  (argc != 2) {
        fprintf(stderr, "input ip\n");
        return 0;
    }

    sock_init(argv[1]);
    time_init();

    sdiscover();
    status = OFFER_WAIT;

    event = 0;
    struct proctable  *pt;


    while(1) {               
        event = wait_event();
        printf("status:%s\n", mytypes[status-1]);

        for (pt = ptab; pt->status; pt++) {
            if (pt->status == status && pt->event == event) {

                (*pt->func)();
                break;
            }
        }
        if (pt->status == 0) {
            fprintf(stderr, "処理が定義されていません\n, status: %s, event:  %s\n", mytypes[status-1], mytypes[event-1]);
        }
    }
    /*
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
                 break;
            case 1:
                srequest();

                receive_print();

                break;
            case 2:
                srequest2();
                receive_print();
                break;   
            default:
                break;
        };
    }
    */
   
    return 0;
}

