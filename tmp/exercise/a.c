#include "buf.h"

int main()
{
	char *my_argv[] = {"hello", NULL};
	execvp("ls", my_argv);

	return 0;
}
