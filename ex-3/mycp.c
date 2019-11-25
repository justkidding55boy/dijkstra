#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXCHAR 256
int main(int argc, char **argv)
{

	int r;
	r = open(argv[1], O_RDONLY);
	char buf[MAXCHAR];
	read(r, buf, 1024);

	int i;
	for (i = 0; buf[i] != '\0'; i++);
	int w;
	w = open(argv[2], O_WRONLY|O_CREAT, 0644);
	write(w, buf, i);

	close(r);
	close(w);

	return 0;
}
