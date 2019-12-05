#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAXCHAR 256
int main(int argc, char **argv)
{

	int r;
	r = open(argv[1], O_WRONLY|O_CREAT);
	char buf[MAXCHAR];
	read(r, buf, 1024);

	printf("%d\n", errno);

	return 0;
}
