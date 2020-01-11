#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv) {

  char destination[80];
  memset(destination, 0, sizeof destination);
  unsigned short port = 50002;
  int dstSocket;

  struct sockaddr_in dstAddr;
  memset(&dstAddr, 0, sizeof(dstAddr));
  char *toSendText = "This is a test";

  if (argc == 2) {
      strncpy(destination, argv[1], strlen(destination));
  } else {
      sprintf(destination, "127.0.0.1");
  } 

  //////////////////////DNS///

  char *node = "logex00.educ.cc.keio.ac.jp/";
  char *serv = "http";
 //////////////////////DNS///


  
  struct in_addr ipaddr;
  memset(&ipaddr, 0, sizeof ipaddr);
  if (inet_aton(destination, &ipaddr) <= 0) {
      fprintf(stderr, "inet_aton: error\n");
      exit(1);
  }
  dstAddr.sin_addr.s_addr = ipaddr.s_addr;

  

  dstAddr.sin_port = htons(port);
  dstAddr.sin_family = AF_INET;

  //dstAddr.sin_addr.s_addr = inet_addr(destination);
  dstSocket = socket(AF_INET, SOCK_STREAM, 0);

  printf("Trying to connect to %s: \n", destination);
  if (connect(dstSocket, (struct sockaddr *) &dstAddr, sizeof(dstAddr)) < 0) {
      perror("connect");
      exit(1);
  }

  int i;
  for(i=0; i<10; i++) {
    printf("sending...\n");
    send(dstSocket, toSendText, strlen(toSendText)+1, 0);
    char buffer[256];
    memset(buffer, 0, sizeof buffer);
    recv(dstSocket, buffer, sizeof(buffer), 0);
    printf("receive:%s\n", buffer);
    sleep(1);
  }

  //close(dstSocket);
  return 0;
}

