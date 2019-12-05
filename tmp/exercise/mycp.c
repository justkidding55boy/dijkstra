#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXCHAR 256
int main(int argc, char **argv)
{

	int r;
	r = open(argv[1], O_RDONLY);

	if ((r = open(argv[1], O_RDONLY)) < 0) {
		if (errno != EEXIST) {
			perror("open");
			exit(1);
		}
	}

	char buf[MAXCHAR];
	memset(buf, 0, MAXCHAR);
	int w;
	w = open(argv[2], O_WRONLY|O_CREAT, 0644);

	while (read(r, buf, MAXCHAR) > 0) {
		int i;
		for (i = 0; buf[i] != '\0'; i++);
		write(w,buf, i);
		memset(buf, 0, MAXCHAR);
	}


	close(r);
	close(w);

	return 0;
}
