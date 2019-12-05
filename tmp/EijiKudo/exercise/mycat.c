#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXCHAR 256

int main(int argc, char *argv[])
{
	int fd;
	char filename[MAXCHAR];
	char buf[MAXCHAR];

	int i;
	for (i = 1; i < argc; i++) {
		memset(filename, 0, MAXCHAR);
		strncpy(filename, argv[i], MAXCHAR-1);
		
		if ((fd = open(filename, O_RDONLY)) < 0) {
			if (errno != EEXIST) {
				perror("open");
				exit(1);
			}
		}


		memset(buf, 0, MAXCHAR);
		while (read(fd, buf, MAXCHAR)  > 0) {
			printf("%s", buf);

			memset(buf, 0, MAXCHAR);
		}

		if (argc > 3)
			printf("\n---------%s-----------\n", filename);

	}

	close(fd);

	return 0;
}
