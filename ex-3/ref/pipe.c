#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    char *argv[] = {"ls", "|", "head" , "|", "wc", NULL};
    int i, pipe_locate[10], pipe_count = 0;
    pipe_locate[0] = -1;
    for (i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "|") == 0) {
            pipe_count++;
            pipe_locate[pipe_count] = i;
            argv[i] = NULL;
        }
    }
	int pfd[9][2];
    if (pipe_count == 0) {
        if (fork() == 0) {
            execvp(argv[0], argv);
            exit(0);
        }
        else {
            int status;
            wait(&status);
        }
    }

    for (i = 0; i < pipe_count + 1 && pipe_count != 0; i++) {
        if (i != pipe_count) pipe(pfd[i]);

        if (fork() == 0) {
            if (i == 0) {
                dup2(pfd[i][1], 1);
                close(pfd[i][0]); close(pfd[i][1]);
            } else if (i == pipe_count) {
                dup2(pfd[i - 1][0], 0);
                close(pfd[i - 1][0]); close(pfd[i - 1][1]);
            } else {
                dup2(pfd[i - 1][0], 0);
                dup2(pfd[i][1], 1);
                close(pfd[i - 1][0]); close(pfd[i - 1][1]);
                close(pfd[i][0]); close(pfd[i][1]);
            }

            execvp(argv[pipe_locate[i] + 1], argv + pipe_locate[i] + 1);
            exit(0);
        }
        else if (i > 0) {
            close(pfd[i - 1][0]); close(pfd[i - 1][1]);
        }
    }
    int status;

    for (i = 0; i < pipe_count + 1; i++) {
        wait(&status);
    }
    return 0;
}

