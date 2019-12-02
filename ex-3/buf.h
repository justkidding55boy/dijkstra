#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define MAXARG 80
#define BUFSIZE 1024
#define ENDWORD "exit"


#define MAX_ARGC 16
#define MAX_ARGV 256
#define MAX_BUF 256
#define MAXCHAR 256
struct command_table {
	char *cmd;
	void (*func)(int, char *[]);
	char *format;
	char *desc;
};

#define TKN_NORMAL        0
#define TKN_REDIR_IN      1
#define TKN_REDIR_OUT     2
#define TKN_REDIR_APPEND  3
#define TKN_PIPE          4
#define TKN_BG            5
#define TKN_EOL           6
#define TKN_EOF           7
#define TKN_NONE          8

#define TOKENLEN 80
#define TOKENNUM 10
