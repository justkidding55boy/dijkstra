

#include "mydhcp.h"

//init
void get_config(char *filename);
void counter_init();
void sock_init();
void time_init();
//init


void soffer(), sstarve(), sack();
void f_act4(), f_act5(), f_act6();
void release(), count_down();

//proc.c
extern void print_buf(struct dhcph);
extern void print_client_list();

uint8_t alrmflag = 0;
uint16_t ttl; //fileに書いてあったtime to live
fd_set  rdfds; //記述子集合
struct timeval tv; //select の監視時間
uint8_t timeoutflag = 0;
struct dhcph rbuf;

int event;
int s;
socklen_t sktlen;
struct sockaddr_in skt; //相手ソケット
int count;
int status;
struct client *p; // 対象のクライアント
struct timeval tv;

void sigh_alrm() {
    alrmflag++;
}
int wait_event();
void print_buf(struct dhcph);

struct proctable {
    int status;
    int event;
    void (*func)();
} ptab [] = {
    {DISCOVER_WAIT, RDISCOVER, soffer},
    {DISCOVER_WAIT, IPSTARVE, sstarve},
    {REQUEST_WAIT, RREQUEST, sack},
    {IP_ALLOC, RREQUEST, sack},
    {FINISH, 0, f_act5},
    {0, 0, NULL}
};
int mycnt = 0;
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "input config file\n");
        return 1;
    }

    get_config(argv[1]);
    sock_init();
    print_client_list();
    time_init();
    
    struct proctable *pt;
    int event;
    p = client_list.fp;

    while (1) {
        event = wait_event();
        printf("event: %d(%s)\n", event, mytypes[event-1]);

        for (pt = ptab; pt->status; pt++) {
            //printf("pt->status:%d(%s), p->status: %d(%s)\n", pt->status, mytypes[pt->status-1], p->status, mytypes[p->status-1]); 
            if (pt->event == event && pt->status == p->status) {
                (*pt->func)();

                break;
            }
        }

        if (event == IPSTARVE) sstarve();

        if (pt->status == 0)    fprintf(stderr, "eventが定義されていません\n");

        print_client_list();


    }

    // skt.sin_port に送信側のポート番号が設定されている 
    // skt.sin_addr.s_addr に送信側の IP アドレスが設定されている 
    // バイトオーダに気をつけること
    // sktlen に相手側ソケットアドレスの実際のサイズが返される
    return 0;
}

//getargs.c
extern void getargs(int *argc, char *argv[], char *buf);
void get_config(char *filename)
{
    //open filename
    //IPアドレスとネットマスクの組みを取得してclientリスト最後尾に追加
    //statを初期化する
    memset(&client_list, 0, sizeof client_list);
    client_list.fp = &client_list;
    client_list.bp = &client_list;

    FILE *fp;
    char buf[MAXCHAR];
    memset(buf, 0, sizeof buf);

    if ((fp = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "file open error\n");
        exit(EXIT_FAILURE);
    }

    if (fgets(buf, MAXCHAR-1, fp) == NULL) {
        fprintf(stderr, "illegal filename:  %s\n", filename);
    }
    int i;
    for (i = 0; buf[i] != '\n'; i++) ;
    buf[i] = '\0';



    if ((ttl = atoi(buf)) <= 0) {
        fprintf(stderr, "illegal timeoutsec\n");
    }
    memset(buf, 0, sizeof buf);

    while(fgets(buf, MAXCHAR-1, fp) != NULL) {
        for (i = 0; buf[i] != '\n'; i++) ;
        buf[i] = '\0';

        int ac;
        char **av;
        av = malloc(sizeof (char *)  * 2);

        for (i = 0; i < 2; i++) {
            av[i] = malloc(sizeof (char) * MAXCHAR);
        }
        getargs(&ac, av, buf);
        memset(buf, 0, sizeof buf);
        if (ac != 2) {
            fprintf(stderr, "illegal pair of IP and netmask\n");
            exit(1);
        } 

        p = malloc(sizeof (struct client));
        memset(p, 0, sizeof (struct client));
        p->fp = &client_list;
        p->bp = client_list.bp;
        client_list.bp->fp = p;
        client_list.bp = p;

        if (inet_aton(av[0], &(p->addr)) == 0) {
            fprintf(stderr, "illegal ip address\n");
            exit(1);
        } 

        if (inet_aton(av[1], &(p->netmask)) == 0) {
            fprintf(stderr, "illegal netmask\n");
            exit(1);
        }

        p->status = DISCOVER_WAIT;

        p = p->fp; 

    }
    fclose(fp);

}

void counter_init()
{
    //timerの初期化
}

void sock_init()
{
    //socketの初期化

    in_port_t myport = 51230;

    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
    }

	struct sockaddr_in myskt; //自ソケット

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
    printf("start\n***************\n\n");
    sktlen = sizeof skt;

}

void time_init()
{
    struct itimerval ttltimer;
    ttltimer.it_value.tv_sec = 1;
    ttltimer.it_value.tv_usec = 0;
    ttltimer.it_interval.tv_sec = 1;
    ttltimer.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL, &ttltimer, NULL) < 0) {
        perror("setitimer");
        exit(1);
    }

}
int check_cli_list(struct sockaddr_in skt);
int wait_event()
{
 
    int retval;
    int time10 = 0;
    uint8_t loop;

    do {    
        signal(SIGALRM, sigh_alrm);
        tv.tv_sec = TIMEOUT;
        tv.tv_usec = 0;
        FD_ZERO(&rdfds);
        FD_SET(s, &rdfds);
        retval = select(s+1, &rdfds, NULL, NULL, &tv);
        errno = loop = 0;

        if (retval > 0) {
            //更新された記述子の数
            if (FD_ISSET(s, &rdfds)) {
                memset(&rbuf, 0, sizeof rbuf);
                if ((count = recvfrom(s, &rbuf, sizeof rbuf, 0, (struct sockaddr *)&skt, &sktlen)) < 0) {
                    perror("recvfrom");
                    exit(1);
                }
                fprintf(stderr, "receive\n");
                print_buf(rbuf);
                // check the client list
                int v;
                if ((v = check_cli_list(skt)) != 0) return v;
                
                // for new clients
                if (rbuf.type != SDISCOVER) {
                    fprintf(stderr, "illegal message from an unknown user\n");
                    continue;
                } else {
                    for (p = client_list.fp; p != &client_list; p = p->fp) {
                        if (p->status == DISCOVER_WAIT) return RDISCOVER;
                        printf("RDISCOVER?%s\n", mytypes[p->status-1]);
                    }
                    if (p == &client_list) 
                        //no IP address for allocatioin
                        return IPSTARVE;
                } 
            }
        } else if (retval  == 0) {
            printf("retval == 0\n");
            //時間切れ
        } else {
            //select no update
            printf("errno:%d\n", errno);
            printf("mycnt:%d\n", mycnt++);

            if (errno == 4) {
                if (alrmflag == 1) {
                    for (p = client_list.fp; p !=  &client_list; p = p->fp) { 
                        if (p->status == IP_ALLOC)  count_down();
                        if (p->status == REQUEST_WAIT) {
                            time10+=1;
                            if (time10 > 10) {
                                fprintf(stderr, "timeout\n");
                                if (timeoutflag) exit(1);
                                else {
                                    timeoutflag += 1;
                                    time10 = 0;
                                    for (p=client_list.fp; p!=&client_list;p=p->fp) {
                                        if (p->status == REQUEST_WAIT) {
                                            p->status--; soffer();
                                        }
                                    }
                                }
                            }
                            break;
                        }
                    }
                }

            } 
            alrmflag = 0;
            loop = 1;
            continue;
        }
        printf("loop:%d\n", loop);

    } while(loop);

}

int check_cli_list(struct sockaddr_in skt)
{

    for (p = client_list.fp; p != &client_list; p =  p->fp) {
        if (p->id.s_addr == skt.sin_addr.s_addr && p->port == ntohs(skt.sin_port)) {
            if (p->status == DISCOVER_WAIT && rbuf.type == SDISCOVER)  return RDISCOVER;
            if (p->status == REQUEST_WAIT  && rbuf.type == SREQUEST)   return RREQUEST;
            if (p->status == IP_ALLOC      && rbuf.type == SREQUEST)   return RREQUEST;
            if (p->status == IP_ALLOC      && rbuf.type == RELEASE)    return RELEASE;
            fprintf(stderr, "Illegal message\n");
            continue;  
        }
    }

    return 0;

}

void soffer()
//discoverに対して(割り当て可能IPアドレスが残っている時)
{
   memset(&rbuf, 0, sizeof rbuf); 
   printf("soffer\n");
   rbuf.type = SOFFER;
   rbuf.code = 0;
   rbuf.ttl = ttl;
   rbuf.address = p->addr.s_addr;
   rbuf.netmask  = p->netmask.s_addr;
   //記録
   p->id.s_addr = skt.sin_addr.s_addr;
   p->port = ntohs(skt.sin_port);

   fprintf(stderr, "send\n");
   print_buf(rbuf);

   
    if ((count = sendto(s, &rbuf, sizeof rbuf, 0, (struct sockaddr *) &skt, sizeof skt)) < 0) {
        perror("sendto");
        exit(1);
    }    
    printf("state change: stat(%s) -> stat(%s)\n", mytypes[p->status-1], mytypes[REQUEST_WAIT-1]);
    p->status = REQUEST_WAIT;
    printf("**\n");

    //eventを返す

}

void f_act101()
//discoverに対して(割り当て可能IPアドレスが残っていなかった時)
{

}

void sstarve()
{
    memset(&rbuf, 0, sizeof rbuf); 
    printf("sstarve\n");
    rbuf.type = SSTARVE;
    rbuf.code = 1;
    rbuf.ttl = ttl;
    rbuf.address = p->addr.s_addr;
    rbuf.netmask  = p->netmask.s_addr;


    fprintf(stderr, "send\n");
    print_buf(rbuf);

   
    if ((count = sendto(s, &rbuf, sizeof rbuf, 0, (struct sockaddr *) &skt, sizeof skt)) < 0) {
        perror("sendto");
        exit(1);
    }    
    printf("**\n");

    
}

void f_act4()
{

}

void remove_and_insert();
void release()
{
    printf("release\n");
    p->id.s_addr = p->port = 0;
    p->status = DISCOVER_WAIT;
    remove_and_insert();

}

void sack()
{
   
   memset(&rbuf, 0, sizeof rbuf); 
   printf("sack\n");
   rbuf.type = SACK;
   if (rbuf.code == 2) {
       //割り当て要求
       rbuf.code = 0;
   

       if  (rbuf.address !=  p->addr.s_addr || rbuf.netmask != p->netmask.s_addr)  rbuf.code = 4;
       if (rbuf.ttl > ttl || rbuf.ttl <= 1)  rbuf.code = 4; //error
       else {
           p->ttl = rbuf.ttl;
           p->ttlcounter =  rbuf.ttl;
       }

       if (rbuf.code == 4) {
           //release
           release();
           rbuf.ttl =  rbuf.address = rbuf.netmask = 0;
       }  else {
           printf("Allocated IP: %s, netmask: %s\n", inet_ntoa(p->addr), inet_ntoa(p->netmask));
           printf("ttl: %d\n", p->ttl);
           printf("To IP(%s), port(%d)\n", inet_ntoa(p->id) , p->port);
       }

   } else {
       rbuf.code = 0;
       rbuf.ttl = ttl;
       rbuf.address = p->addr.s_addr;
       rbuf.netmask  = p->netmask.s_addr;
       p->ttlcounter = p->ttl;
   }
   //記録
   p->id.s_addr = skt.sin_addr.s_addr;
   p->port = ntohs(skt.sin_port);

   fprintf(stderr, "send\n");
   print_buf(rbuf);

   
    if ((count = sendto(s, &rbuf, sizeof rbuf, 0, (struct sockaddr *) &skt, sizeof skt)) < 0) {
        perror("sendto");
        exit(1);
    }    
    printf("state change: stat(%s) -> stat(%s)\n", mytypes[p->status-1], mytypes[IP_ALLOC-1]);
    p->status = IP_ALLOC;
    printf("**\n");


}
void f_act5()
{

}
void f_act6()
{


}

void count_down()
{
    if (p->ttl > 1)
        printf("%s: remaining;%ds\n", inet_ntoa(p->addr), --(p->ttlcounter));
    else {
        printf("%s: expired\n", inet_ntoa(p->addr));
        release();
    }
}

void remove_and_insert()
{
    p->bp->fp = p->fp;
    p->fp->bp = p->bp;
    p->fp = &client_list;
    p->bp = client_list.bp;
    client_list.bp->fp = p;
    client_list.bp = p;
}


