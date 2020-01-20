#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "myftph.h"

#define BUFSIZE 256
#define NARGS   32

void quit(int, int, char *[]), pwd(int, int, char *[]), cd(int, int, char *[]);
void dir(int, int, char *[]), lpwd(int, int, char *[]), lcd(int, int, char *[]);
void ldir(int, int, char *[]), get(int, int, char *[]), put(int, int, char *[]);
void help(int, int, char *[]);

void getargs(char *, int *, char *[]);
void show_fileinfo(char *, char *);

struct cmdtable {
  char *cmd;
  void (*func)(int, int, char *[]);
} cmdtab[] = {
  {"quit",    quit},
  {"pwd",     pwd},
  {"cd",      cd},
  {"dir",     dir},
  {"lpwd",    lpwd},
  {"lcd",     lcd},
  {"ldir",    ldir},
  {"get",     get},
  {"put",     put},
  {"help",    help},
  {NULL,      NULL}
};


int main(int argc, char *argv[])
{
  char buf[BUFSIZE], *av[NARGS], srv[BUFSIZE];
  int s, err, ac, i;
  struct cmdtable *cp;
  struct sockaddr_in srvskt;
  struct addrinfo hints, *res, *p;

  if (argc != 2) {
    fprintf(stderr, "input server name\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_socktype = SOCK_STREAM;
  sprintf(srv, "%d", SRVPORT);

  if ((err = getaddrinfo(argv[1], srv, &hints, &res)) < 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
    exit(1);
  }

  for (p = res; p; p = p -> ai_next) {
    if (p -> ai_family == PF_INET) break;
  }
  if (p == NULL) {
    fprintf(stderr, "server name error: %s\n", argv[1]);
    freeaddrinfo(res);
    exit(1);
  }

  if ((s = socket(p -> ai_family, p -> ai_socktype, p -> ai_protocol)) < 0) {
    perror("socket");
    exit(1);
  }
  if (connect(s, p -> ai_addr, p -> ai_addrlen) < 0) {
    perror("connect");
    exit(1);
  }
  freeaddrinfo(res);

  for (;;) {
    fprintf(stderr, "myFTP$ ");
    if (fgets(buf, sizeof buf, stdin) == 0) {
      if (ferror(stdin)) {
        perror("fgets");
        exit(1);
      }
      exit(0);
    }
    i = strlen(buf);
    buf[i - 1] = '\0';
    getargs(buf, &ac, av);
    if (ac == 0) continue;

    for (cp = cmdtab; cp -> cmd; cp++) {
      if (strcmp(av[0], cp -> cmd) == 0) {
        (*cp -> func)(s, ac, av);
        break;
      }
    }
    if (cp -> cmd == NULL) {
      fprintf(stderr, "unknpwn command: %s\n", av[0]);
    }

  }

  return 0;
}

void getargs(char *p, int *ac, char *av[])
{
  *ac = 0;
  av[0] = NULL;

  for (;;) {
    while (isblank(*p)) {
      p++;
    }
    if (*p == '\0') {
      return;
    }
    av[(*ac)++] = p;
    while (*p && !isblank(*p)) {
      p++;
    }
    if (*p == '\0') {
      return;
    }
    *p++ = '\0';
  }
}
