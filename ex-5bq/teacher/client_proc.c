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

void quit(int s, int ac,char *av[])
{
  struct myftph ftph;

  ftph.type = T_QUIT;
  ftph.code = 0;
  ftph.length = htons(0);
  if (send(s, &ftph, sizeof ftph, 0) != sizeof ftph) {
    perror("send");
    exit(1);
  }
  exit(0);
}

void pwd(int s, int ac, char *av[])
{
  struct myftph *fhp;
  char pkt[PKTSIZE], *dp;
  int c, datalen;

  if (ac != 1) {
    fprintf(stderr, "invalid argument\n");
    return;
  }

  fhp = (struct myftph *)pkt;
  dp = (char *)(fhp + 1);
  fhp -> type = T_PWD;
  fhp -> code = 0;
  int t;
  if ((t = send(s, pkt, sizeof (struct myftph), 0)) != (sizeof (struct myftph))) {
    perror("send");
    exit(1);
  }
  printf("send:%dbyte myftph:%d\n", t, sizeof(struct myftph));

  if ((t = recv(s, pkt, sizeof (struct myftph), 0)) != (sizeof (struct myftph))) {
    perror("recv");
    exit(1);
  }
  printf("receive:%d\n", t);
  printf("r:%dbyte myftph:%d\n", t, sizeof(struct myftph));
  if (fhp -> type != T_OK) {
    fprintf(stderr, "PWD command failed.\n");
    return;
  }

  datalen = ntohs(fhp -> length);

  if (recv(s, dp, datalen, 0) != datalen) {
    perror("recv");
    exit(1);
  }

  dp[ntohs(fhp -> length)] = '\0';
  printf("%s\n", dp);
}

void cd(int s, int ac, char *av[])
{
  char pkt[PKTSIZE], *dp;
  struct myftph *fhp;
  int msglen;

  if (ac != 2) {
    fprintf(stderr, "path name error.\n");
    return;
  }

  fhp = (struct myftph *)pkt;
  dp = (char *)(fhp + 1);
  fhp -> type = T_CWD;
  fhp -> code;
  fhp -> length = htons(strlen(av[1]));
  strcpy(dp, av[1]);
  msglen = sizeof (struct myftph) + strlen(av[1]);

  if (send(s, pkt, msglen, 0) != msglen) {
    perror("send");
    exit(1);
  }

  if (recv(s, pkt, sizeof (struct myftph), 0) != sizeof (struct myftph)) {
    perror("send");
    exit(1);
  }

  if (fhp -> type == T_OK){
    return;
  } else {
    fprintf(stderr, "cd error.\n");
  }
}

void dir(int s, int ac, char *av[])
{
  char pkt[PKTSIZE], *dp;
  struct myftph *fhp;
  int msglen;

  if (ac != 1 && ac != 2) {
    fprintf(stderr, "argument error.\n");
  }
  fhp = (struct myftph *)pkt;
  dp = (char *)(fhp + 1);
  fhp -> type = T_LIST;
  fhp -> code = 0;
  fhp -> length = htons(0);
  msglen = sizeof (struct myftph);

  if (ac == 2) {
    msglen += strlen(av[1]);
    strcpy(dp, av[1]);
    fhp -> length = htons(strlen(av[1]));
  }

  if (send(s, pkt, msglen, 0) != msglen) {
    perror("send");
    exit(1);
  }

  if (recv(s, pkt, sizeof (struct myftph), 0) != sizeof (struct myftph)) {
    perror("recv");
    exit(1);
  }

  if (fhp -> type != T_OK) {
    fprintf(stderr, "dit error.\n");
    return;
  }

  if (fhp -> code != C_DATA_SC) {
    fprintf(stderr, "illegal code dir\n");
    return;
  }

  do {
    if (recv(s, pkt, sizeof (struct myftph), 0) != sizeof (struct myftph)) {
      perror("recv");
      exit(1);
    }

    if (fhp -> type != T_DATA) {
      fprintf(stderr, "dit error.\n");
      return;
    }
    msglen = ntohs(fhp -> length);

    if (msglen > 0) {
      if (recv(s, dp, msglen, 0) != msglen) {
        perror("recv");
        exit(1);
      }
      dp[msglen] = '\0';
      printf("%s\n", dp);
    }
  } while (fhp -> code == C_DATA_CONT);

  return;
}

void lpwd(int s, int ac, char *av[])
{
  char buf[BUFSIZE];

  if (ac != 1) {
    fprintf(stderr, "lpwd error.\n");
    return;
  }

  if (getcwd(buf, sizeof buf) == NULL) {
    perror("getcwd");
  }
  printf("%s\n", buf);
}
void lcd(int s, int ac, char *av[])
{
  if (ac != 2) {
    fprintf(stderr, "lcd error.\n");
    return;
  }

  if (chdir(av[1]) < 0) {
    perror("chdir");
  }
  return;
}

void show_fileinfo(char *dname, char *fname)
{
  struct stat statval;
  struct tm tmval;
  char pname[BUFSIZE];

  sprintf(pname, "%s%s", dname, fname);
  if (stat(pname, &statval) < 0) {
    perror("stat");
    return;
  }
  localtime_r(&statval.st_mtime, &tmval);
  printf("%ld\t%d.%02d.%02d %02d:%02d\t%s%c\n",
          statval.st_size, tmval.tm_year+1900, tmval.tm_mon+1,
          tmval.tm_mday, tmval.tm_hour, tmval.tm_min, fname,
          S_ISDIR(statval.st_mode)? '/':' ');

}

void ldir(int s, int ac, char *av[])
{
  char dname[BUFSIZE];
  struct stat statval;
  struct tm tmval;
  DIR *dir;
  struct dirent *dp;

  if (ac == 1) {
    strcpy(dname, "./");
  } else {
    strcpy(dname, av[1]);
  }

  if (stat(dname, &statval) < 0) {
    perror("stat");
    return;
  }
  if (S_ISDIR(statval.st_mode)) {
    if ((dir = opendir(dname)) == NULL) {
      perror("opendir");
      return;
    }
    while (dp = readdir(dir)) {
      if (dp -> d_name[0] == '.') continue;
      show_fileinfo(dname, dp -> d_name);
    }
    closedir(dir);
    return;
  }

  if (S_ISREG(statval.st_mode)) {
    localtime_r(&statval.st_mtime, &tmval);
    printf("%ld\t%d.%02d.%02d %02d:%02d\t%s\n",
            statval.st_size, tmval.tm_year+1900, tmval.tm_mon+1,
            tmval.tm_mday, tmval.tm_hour, tmval.tm_min, dname);
    return;
  }
}
void get(int s, int ac, char *av[])
{
  int fd, msglen, datalen, cnt, remain, totallen;
  char *f1, *f2, pkt[PKTSIZE], *dp, *p;
  struct myftph *fhp;

  if (ac != 2 && ac != 3) {
    fprintf(stderr, "file_name?\n");
    return;
  }
  if (ac == 2) {
    f1 = f2 = av[1];
  } else {
    f1 = av[1];
    f2 = av[2];
  }

  if ((fd = open(f2, O_WRONLY|O_CREAT|O_TRUNC, 0644)) < 0) {
    perror("open");
    return;
  }

  fhp = (struct myftph *)pkt;
  dp = (char *)(fhp + 1);
  fhp -> type = T_RETR;
  fhp -> code = 0;
  fhp -> length = htons(strlen(f1));
  strcpy(dp ,f1);
  msglen = sizeof(struct myftph) + strlen(f1);
  // fprintf(stderr, "DEBUG fname: %s\n", dp);
  if (send(s, pkt, msglen, 0) != msglen) {
    perror("send");
    exit(1);
  }

  if (recv(s, pkt, sizeof (struct myftph), 0) != sizeof (struct myftph)) {
    perror("recv");
    exit(1);
  }

  //revv reply, error staff
  if (fhp -> type != T_OK) {
    fprintf(stderr, "illeal type: %d\n", fhp -> type);
    close(fd);
    unlink(f2);
    return;
  }
  if (fhp -> code != C_DATA_SC) {
    fprintf(stderr, "illeal type: %d\n", fhp -> type);
    close(fd);
    unlink(f2);
    return;
  }

  totallen = 0;
  do {
    if (recv(s, pkt, sizeof (struct myftph), 0) != sizeof (struct myftph)) {
      perror("recv");
      exit(1);
    }
    if (fhp -> type != T_DATA) {
      fprintf(stderr, "illeal type: %d\n", fhp -> type);
      return;
    }
    datalen = ntohs(fhp -> length);
    totallen += datalen;
    // fprintf(stderr, "recv: %d bytes to kaitearuyatu\n", datalen);

    if (datalen > 0) {
      p = dp;
      remain = datalen;
      do {
        if ((cnt = recv(s, p, remain, 0)) < 0) {
          perror("recv");
          exit(1);
        }
        p += cnt;
        remain -= cnt;

        if (remain > 0) {
          // fprintf(stderr, "partial recv: %d/%d\n", cnt, remain+cnt);
        }
      } while (remain > 0);
      if (write(fd, dp, datalen) != datalen) {
        perror("write");
        exit(1);
      }
    }

  } while (fhp -> code == C_DATA_CONT);

  close(fd);
  printf("%d bytes received.\n", totallen);

}
void put(int s, int ac, char *av[])
{
  int fd, msglen, datalen, cnt, remain, totallen;
  char *f1, *f2, pkt[PKTSIZE], *dp, *p;
  struct myftph *fhp;

  if (ac != 2 && ac != 3) {
    fprintf(stderr, "file_name?\n");
    return;
  }
  if (ac == 2) {
    f1 = f2 = av[1];
  } else {
    f1 = av[1];
    f2 = av[2];
  }

  if ((fd = open(f1, O_RDONLY)) < 0) {
    perror("open");
    return;
  }

  fhp = (struct myftph *)pkt;
  dp = (char *)(fhp + 1);
  fhp -> type = T_STOR;
  fhp -> code = 0;
  fhp -> length = htons(strlen(f2));
  strcpy(dp ,f2);
  // fprintf(stderr, "dp: %s\n", dp);
  msglen = sizeof(struct myftph) + strlen(f2);
  if (send(s, pkt, msglen, 0) != msglen) {
    perror("send");
    exit(1);
  }

  if (recv(s, pkt, sizeof (struct myftph), 0) != sizeof (struct myftph)) {
    perror("recv");
    exit(1);
  }

  //revv reply, error staff
  if (fhp -> type != T_OK) {
    fprintf(stderr, "illegal type: %d\n", fhp -> type);
    close(fd);
    unlink(f2);
    return;
  }
  if (fhp -> code != C_DATA_CS) {
    fprintf(stderr, "illegal type: %d\n", fhp -> type);
    close(fd);
    unlink(f2);
    return;
  }
  totallen = 0;
  // fprintf(stderr, "REPLY OK!!\n");
  //data msg
  int maxsize = PKTSIZE - sizeof(struct myftph); //1024
  fhp -> type = T_DATA;
  while ((cnt = read(fd, dp, maxsize)) == maxsize) {
    fhp -> code = C_DATA_CONT;
    fhp -> length = htons(cnt);
    msglen = sizeof(struct myftph) + cnt;
    totallen += cnt;
    // fprintf(stderr, "send T_DATA\n");
    if (send(s, pkt, msglen, 0) != msglen) {
      perror("send");
      exit(1);
    }
  }
  if (cnt < 0) {
    perror("read");
    close(fd);
    exit(1);
  }
  else {
    fhp -> code = C_DATA_END;
    fhp -> length = htons(cnt);
    msglen = sizeof(struct myftph) + cnt;
    totallen += cnt;
    if (send(s, pkt, msglen, 0) != msglen) {
      perror("send");
      exit(1);
    }
  }

  close(fd);
  printf("%d bytes sent.\n", totallen);
}
void help(int s, int ac, char *av[])
{
  printf("H E L P !!\n");
}
