
#include "buf.h"
#define MAXCHAR 256
#define MAXCOM 10
//input.c
extern void getargs(int *, char **, char *);

//proc.c
extern void quit_proc(int, char**);
extern int cd_proc(int, char**);

//gettoken.c
extern int gettoken(char *token, int len, char *buf);
extern char* pr_ttype(int ttype);
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

		int dirflg = 0;
		//classify the types
		char token[TOKENLEN];
		int ttype[TOKENNUM];
		for (i = 0; i < my_argc; i++) {
			if (ttype[i] == TKN_REDIR_IN || ttype[i] == TKN_REDIR_OUT || ttype[i] == TKN_REDIR_APPEND) {
				dirflg = 1;

			}
			if (my_argv[i] == NULL) {
				//finish
				ttype[i] = gettoken(token, TOKENLEN, "\n");
				break;
			}

			ttype[i] = gettoken(token, TOKENLEN, my_argv[i]);
		}


		// if cd or quit, don't conduct execvp. 
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
		// if cd or quit, don't conduct execvp.
 
		flg = 0;
        if (strcmp(my_argv[0], ENDWORD) == 0)
            return 0;

		int pipe_locate[10], pipe_count = 0;
		pipe_locate[0] = -1;
		for (i = 0; i < my_argc;i++) {
			if (strcmp(my_argv[i], "|") == 0) {
				pipe_count++;
				pipe_locate[pipe_count] = i;
				my_argv[i] = NULL;
			}
		}
		int pfd[9][2];
		for (i = 0; i < pipe_count+1 && pipe_count != 0; i++) {
			flg = 1;
			if (i != pipe_count) pipe(pfd[i]);
			if (fork() == 0) {
				if (i == 0) {
					// to the stdout
					dup2(pfd[i][1], 1);
					//close the stdin and stdout
					close(pfd[i][0]); 
					close(pfd[i][1]);
				} else if (i == pipe_count) {
					//to the stdin
					dup2(pfd[i-1][0], 0);
					close(pfd[i-1][0]); close(pfd[i-1][1]);
				} else {
					dup2(pfd[i-1][0], 0);
					dup2(pfd[i][1], 1);
					close(pfd[i-1][0]); close(pfd[i-1][1]);
					close(pfd[i][0]); close(pfd[i][1]);
				}
				
				execvp(my_argv[pipe_locate[i] + 1], my_argv +pipe_locate[i] + 1);
				exit(0);
			} else if (i > 0) {
				close(pfd[i-1][0]); close(pfd[i-1][1]);
			}

		}

		int status;

		for (i = 0; i < pipe_count + 1; i++) {
			wait(&status);
		}

		/*
		if (pipe_count > 0) {

			for (i = 0; i < MAX_ARGC; i++) {
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
				free(my_argv[i]);
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			}
			continue;
		}*/



		if (pipe_count == 0 || dirflg == 1) {

        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // child process
			/*
            fprintf(stderr, "\t** child: execute %s **\n", my_argv[0]);*/

			
			for (i = 0; i < my_argc; i++) {
				//printf("ttype:%s\n", pr_ttype(ttype[i]));
				

				// > and >>
				if (ttype[i] == TKN_REDIR_OUT || ttype[i] == TKN_REDIR_APPEND) {
	
					//openのエラー処理後で
					int fd;
					if (ttype[i] == TKN_REDIR_OUT)
						fd = open(my_argv[i+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
					if (ttype[i] == TKN_REDIR_APPEND)
						fd = open(my_argv[i+1], O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0644);

	
					if (fd < 0) {
						perror("open");
						exit(1);
					}
					close(1);
					dup(fd);
					close(fd);
					my_argv[i] = NULL;

				}

				// <
				if (ttype[i] == TKN_REDIR_IN) {
					int fd = open(my_argv[i+1], O_RDONLY);
					if (fd < 0) {
						perror("open");
						exit(1);
					}
					close(0);
					dup(fd);
					close(fd);
					my_argv[i] = NULL;
				}


			}


			/*
			if (flg == 1) {
				exit(1);
			}*/



            if ((pipe_count == 0)&&(dirflg!=1)&&(execvp(my_argv[0], my_argv) < 0)) {
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


	}
	return 0;
}
