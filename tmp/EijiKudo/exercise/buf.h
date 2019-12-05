#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
struct command_table {
	char *cmd;
	int (*func)(int, char *[]);
	char *format;
	char *desc;
};
