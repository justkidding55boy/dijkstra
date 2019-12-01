#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>


#define MAXARG 80
#define BUFSIZE 1024
#define ENDWORD "exit"


#define MAX_ARGC 16
#define MAX_ARGV 256
#define MAX_BUF 256

extern void getargs(int *, char **, char *);

int main() 
{
	int pid, ac, stat;
    int i;

    for(;;) {
		char **av, buf[BUFSIZE];

		av = malloc(sizeof(char *) * MAX_ARGC);
		for (i = 0; i < MAX_ARGC; i++) {
			av[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
		}
        fprintf(stderr, "simplesh$ ");
        if (fgets(buf, sizeof buf, stdin) == NULL) {
            if (ferror(stdin)) {
                perror("fgets");
                exit(1);
            }
            exit(0);
        }
        buf[strlen(buf) - 1] = '\0';
        memset(av, 0, MAX_ARGC * MAX_ARGV-1);
        getargs(&ac, av, buf);

        fprintf(stderr, "\t** argv start ** \n");
        for (i = 0; i < ac; i++)
            fprintf(stderr, "\tav[%d]: %s\n", i, av[i]);
        fprintf(stderr, "\t** argv end **\n");

        if (ac == 0)
            continue;
        if (strcmp(av[0], ENDWORD) == 0)
            return 0;
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // child process
            fprintf(stderr, "\t** child: execute %s **\n", av[0]);

            if (execvp(av[0], av) < 0) {
                perror("execvp");
                exit(1);
            }
        }
        // parent process
        fprintf(stderr, "\t** parent: waif for child(%d) **\n", pid);
        if (wait(&stat) < 0) {
            perror("wait");
            exit(1);
        }
        fprintf(stderr, "\t** parent: wait end: %d **\n", stat);
		for (i = 0; i < MAX_ARGC; i++) {
			free(av[i]);
			av[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
		}
		free(*av);

    }
}
