#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#define MAXARG 80
#define BUFSIZE 1024
#define ENDWORD "exit"


#define MAX_ARGC 16
#define MAX_ARGV 256
#define MAX_BUF 256
struct command_table {
	char *cmd;
	int (*func)(int, char *[]);
	char *format;
	char *desc;
};
