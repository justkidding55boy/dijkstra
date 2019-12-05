#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char *newargv[] = { "/Users/eijikudo/program/unix/ex-3/exercise/hello", NULL };
    char *newenviron[] = { NULL };

	/*
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file-to-exec>\n", argv[0]);
        exit(EXIT_FAILURE);
    }*/


	

    execve(newargv[0], newargv, newenviron);
    //execvp("ls", newargv);

    //perror("execve");   /* execve() returns only on error */
    exit(EXIT_FAILURE);
}
