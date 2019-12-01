
#include "buf.h"
#define MAXCHAR 256
#define MAXCOM 10
//input.c
extern void getargs(int *, char **, char *);

//proc.c
extern void quit_proc(int, char**);
extern int cd_proc(int, char**);
struct command_table cmd_tbl[] = {
	{"exit", quit_proc},
	{"quit", quit_proc},
	{"cd", cd_proc},
	{NULL, NULL}
};

int main()
{

	while (1) {

		int my_argc, stat, pid;
		char **my_argv;
		
		//initialization
		my_argc = 0;
		my_argv = malloc(sizeof(char*) * MAXCOM);
		int i;
		for (i = 0; i < MAXCOM; i++) {
			my_argv[i] = malloc(sizeof(char) * MAXCHAR);
		}
		
		char path[MAXPATHLEN];
		if(!getcwd(path, sizeof(path)))
			return -1;
		fprintf(stderr, "%s:", path);

		char buf[MAXCHAR] = {'\0'};


		FILE *fp = stdin;
		

		if (fgets(buf, MAXCHAR-1, fp) == NULL) {
	
			if (ferror(fp)) {
				fprintf(stderr, "input error in fgets()\n");
				return -1;
			} else
				return 0;
		}	

		buf[strlen(buf)-1] = '\0';
		if (strlen(buf) == 0) {
			for (i = 0; i < MAX_ARGC; i++) {
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
				free(my_argv[i]);
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			}

			continue;
		}
		getargs(&my_argc, my_argv, buf);

		int flg = 0;
		struct command_table *p;
		for (p = cmd_tbl; p->cmd; p++) {
			if (strcmp(my_argv[0], p->cmd) == 0) {
				(*p->func)(my_argc, my_argv);
				flg = 1;
				break;
			}
		}	
		if (flg == 1) {
			for (i = 0; i < MAX_ARGC; i++) {
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
				free(my_argv[i]);
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			}
			continue;
		}
 
        if (strcmp(my_argv[0], ENDWORD) == 0)
            return 0;
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // child process
			/*
            fprintf(stderr, "\t** child: execute %s **\n", my_argv[0]);*/

            if (execvp(my_argv[0], my_argv) < 0) {
                perror("execvp");
                exit(1);
            }
        }
        // parent process
		// pid: process ID
		/*
        fprintf(stderr, "\t** parent: wait for child(%d) **\n", pid);*/
        if (wait(&stat) < 0) {
            perror("wait");
            exit(1);
        }
		//stat: stat from child process 
        /*fprintf(stderr, "\t** parent: wait end. stat: %d **\n", stat);*/
		for (i = 0; i < MAX_ARGC; i++) {
			my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			free(my_argv[i]);
			my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
		}


	}
	return 0;
}
