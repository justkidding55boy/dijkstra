
#include "buf.h"
#define MAXCHAR 256
#define MAXCOM 10
//input.c
extern void getargs(int *, char **, char *);

//proc.case
extern int proc_check(int, char**);

//gettoken.c
extern int gettoken(char *token, int len);
extern char* pr_ttype(int ttype);


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

		char c;
		if ((c = getchar()) == '\n') {
			for (i = 0; i < MAX_ARGC; i++) {
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
				free(my_argv[i]);
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			}

			continue;
		}
		ungetc(c, stdin);

		int dirflg = 0;
		//classify the types
		char token[MAX_ARGV][TOKENLEN];
		int ttype[TOKENNUM];
		memset(ttype, -1, TOKENNUM);

		i = 0;
		while(1) {
			ttype[i] = gettoken(token[i], TOKENLEN);
			// fprintf(stderr, "token[%d]:%s\n", i, token[i]);
			strcpy(my_argv[i], token[i]);
			if (ttype[i] == TKN_REDIR_IN || ttype[i] == TKN_REDIR_OUT || ttype[i] == TKN_REDIR_APPEND)
				dirflg = 1;
			if (ttype[i] == TKN_EOL || ttype[i] == TKN_EOF) {
				my_argv[i] = NULL;
				my_argc = i;
				break;
			}
			i++;
		}

	//	if (ttype[i] != TKN_EOL) break;

		// if cd or quit, don't conduct execvp.
		if (proc_check(my_argc, my_argv) == 1) {
			continue;
		}
		// if cd or quit, don't conduct execvp.

		int flg = 0;
        if (strcmp(my_argv[0], ENDWORD) == 0)
            return 0;

		int pipe_locate[10], pipe_count = 0;
		pipe_locate[0] = -1;
		for (i = 0; i < my_argc;i++) {
			if (ttype[i] == TKN_PIPE) {
				pipe_count++;
				pipe_locate[pipe_count] = i;
				my_argv[i] = NULL;
			}
		}
		if (pipe_count > 0) {
			int pfd[9][2];
			for (i = 0; i < pipe_count+1 && pipe_count != 0; i++) {
				flg = 1;
				if (i != pipe_count) pipe(pfd[i]);
				if (fork() == 0) {
					//child process
					if (i == 0) {
						// to the stdout
						dup2(pfd[i][1], 1);
						//close the stdin and stdout
						close(pfd[i][0]);
						close(pfd[i][1]);
						if (dirflg == 1) {
							// <
							int j;
							for (j = 0; j < my_argc; j++) {
								if (ttype[j] == TKN_REDIR_IN) {
									int fd = open(my_argv[j+1], O_RDONLY);
									// fprintf(stderr, "my_argv[i+1]:%s\n", my_argv[j+1]);
									if (fd < 0) {
										perror("open");
										exit(1);
									}
									close(0);
									dup(fd);
									close(fd);
									my_argv[j] = NULL;
								}
							}
						}

					} else if (i == pipe_count) {
						//to the stdin
						dup2(pfd[i-1][0], 0);
						close(pfd[i-1][0]); close(pfd[i-1][1]);
						if (dirflg == 1) {
							int j;
							for (j = 0; j < my_argc; j++) {
								// > and >>
								if (ttype[j] == TKN_REDIR_OUT || ttype[j] == TKN_REDIR_APPEND) {
									//openのエラー処理後で
									int fd;
									if (ttype[j] == TKN_REDIR_OUT)
										fd = open(my_argv[j+1], O_WRONLY|O_CREAT|O_TRUNC, 0644);
									if (ttype[j] == TKN_REDIR_APPEND)
										fd = open(my_argv[j+1], O_WRONLY|O_APPEND|O_CREAT|O_TRUNC, 0644);
									//
									// fprintf(stderr, "open:%s\nto:%s\n argv[%d]\n", my_argv[j+1], my_argv[pipe_locate[i]+1],j);
									// fprintf(stderr, "i:%d\n", i);

									if (fd < 0) {
										perror("open");
										exit(1);
									}

									dup2(fd, 1);
									close(fd);
									my_argv[j] = NULL;

								}

							}
							/*
							for (j = 0; j < MAX_ARGC; j++) {
								my_argv[j] = (char *)malloc(sizeof(char) * MAX_ARGV);
								free(my_argv[i]);
								my_argv[j] = (char *)malloc(sizeof(char) * MAX_ARGV);
							}*/

						}

					} else {
						dup2(pfd[i-1][0], 0);
						dup2(pfd[i][1], 1);
						close(pfd[i-1][0]); close(pfd[i-1][1]);
						close(pfd[i][0]); close(pfd[i][1]);
					}

					// fprintf(stderr, "execvp:%s\n", my_argv[pipe_locate[i]+1]);
					execvp(my_argv[pipe_locate[i] + 1], my_argv +pipe_locate[i] + 1);
					exit(0);
				} //以下 parents
				 else if (i > 0) {
					close(pfd[i-1][0]); close(pfd[i-1][1]);
				}

			}

			int status;

			for (i = 0; i < pipe_count + 1 && pipe_count != 0; i++) {
				wait(&status);
			}

		}

		if (pipe_count == 0) {


			// if pipe_cnt == 0 and dirflg == 1
			if ((pid = fork()) < 0) {
				perror("fork");
				exit(1);
			}

			if (pid == 0) {
				// child process
				if (dirflg == 1) {
					for (i = 0; i < my_argc; i++) {

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
				}


				//fprintf(stderr, "pipe_cnt: %d, dirflg: %d\n", pipe_count, dirflg);
//				if ((pipe_count == 0)&&(dirflg == 0)) {
					if (execvp(my_argv[0], my_argv) < 0) {
						perror("execvp");
						exit(1);
					}
//				}
			} else {
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


	}
	return 0;
}
