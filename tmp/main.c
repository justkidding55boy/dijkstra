//61706613 Eiji Kudo
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
int killflg = 0;
int cpid = 0;
int pipe_locate[10], pipe_count = 0;

//ls.c
extern int myexecve(char *, char *[], char*ep[]);
void abrt_handler(int flg, siginfo_t *info, void *ctx)
{
	// printf("si_signo:%d\nsi_code:%d\n", info->si_signo, info->si_code);
	// printf("si_pid:%d\nsi_uid:%d\n", (int)info->si_pid, (int)info->si_uid);
	// fprintf(stderr, "abrd_handler\n");
	// fprintf(stderr, "cpid:%d\n", cpid);
	killflg = 1;
	if (cpid != 0) {

		int attempt = pipe_count > 0 ? pipe_count : 1;
		int i;
		for (i = 0; i < attempt; i++) {
			killpg(cpid + i, SIGKILL);
			int s;
			waitpid(cpid + i, &s, 0);
		}

	} else if (cpid == 0) {
		return ;
	}

}

int tstp_cpid[10];
int tstp_cnt = 0;
void tstp_handler(int flg, siginfo_t *info, void *ctx)
{
	if (cpid != 0) {
		//また再開できるように保存しておく

		tstp_cpid[tstp_cnt++] = cpid;
		killpg(cpid, SIGTSTP);
	} else if (cpid == 0) {
		return ;
	}
}

void fg_proc()
{
	if (tstp_cnt > 0) {
		cpid = tstp_cpid[--tstp_cnt];


		killpg(tstp_cpid[tstp_cnt], SIGCONT);
		int s;
		waitpid(tstp_cpid[tstp_cnt], &s, 0);

	} else {
		fprintf(stderr, "no stopped processes\n");
	}
}
int bg_pid[10];
int bg_pid_cnt = 0;


int main(int argc, char **argv, char *envp[])
{

	// sigset_t newset;
	// sigemptyset(&newset);
	// sigaddset(&newset, SIGINT);
	// sigprocmask(SIG_BLOCK, &newset, NULL);
	memset(tstp_cpid, 0, 10);
	struct sigaction sa_sigabrt;
	memset(&sa_sigabrt, 0, sizeof sa_sigabrt);
	sa_sigabrt.sa_sigaction = abrt_handler;
	sa_sigabrt.sa_flags = SA_NOCLDWAIT;
	if (sigaction(SIGINT, &sa_sigabrt, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
	sa_sigabrt.sa_sigaction = tstp_handler;
	if (sigaction(SIGTSTP, &sa_sigabrt, NULL) < 0) {
		perror("sigaction");
		exit(1);
	}
	int bg_done_flg;
	while (1) {

		int my_argc, stat;
		//int pid;
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

		char c;
		if ((c = getchar()) == '\n') {
			for (i = 0; i < MAX_ARGC; i++) {
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
				free(my_argv[i]);
				my_argv[i] = (char *)malloc(sizeof(char) * MAX_ARGV);
			}

			continue;
		} else {
			ungetc(c, stdin);
		}

		int dirflg = 0;
		int bgflg = 0;
		//classify the types
		char token[MAX_ARGV][TOKENLEN];
		int ttype[TOKENNUM];
		memset(ttype, -1, TOKENNUM);

		int parent_pgid = getpid();

		i = 0;
		while(1) {
			ttype[i] = gettoken(token[i], TOKENLEN);
			// fprintf(stderr, "token[%d]:%s\n", i, token[i]);
			// fprintf(stderr, "ttype[%d]:%s\n", i, pr_ttype(ttype[i]));

			strcpy(my_argv[i], token[i]);
			if (ttype[i] == TKN_REDIR_IN || ttype[i] == TKN_REDIR_OUT || ttype[i] == TKN_REDIR_APPEND)
				dirflg = 1;
			if (ttype[i] == TKN_BG) {
				bgflg = 1;
				my_argv[i] = NULL;
			}

			if (ttype[i] == TKN_EOL || ttype[i] == TKN_EOF) {
				my_argv[i] = NULL;
				my_argc = i;
				break;
			}
			i++;
		}

		if (ttype[0] == TKN_EOF || ttype[0] == TKN_EOL) {
			continue;
		}

		// if cd or quit, don't conduct execvp.
		if (proc_check(my_argc, my_argv) == 1) {
			continue;
		}
		// if cd or quit, don't conduct execvp.

		int flg = 0;
        if (strcmp(my_argv[0], ENDWORD) == 0)
            return 0;

		pipe_count = 0;
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
			int pgid = 0;
			int pid;
			for (i = 0; i < pipe_count+1 && pipe_count != 0; i++) {
				flg = 1;
				if (i != pipe_count) pipe(pfd[i]);
				pid = fork();
				if (pid == 0) {
					//child process

					if (i == 0) {
						pgid = getpid();
						cpid = pid;
						if (setpgid(pgid, pgid) < 0) {
							perror("setpgid");
							exit(1);
						}

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
						pgid = getpid() - i;
						if (setpgid(getpid(), pgid) < 0) {
							perror("setpgid");
							exit(1);
						}
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
									if (ttype[j] == TKN_REDIR_APPEND) {
										fd = open(my_argv[j+1], O_WRONLY|O_APPEND|O_CREAT, 0644);
										// fprintf(stderr, "APPEND\n");
									}


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
						}

					} else {
						pgid = getpid() - i;

						if (setpgid(getpid(), pgid) < 0) {
							perror("setpgid");
							exit(1);
						}
						dup2(pfd[i-1][0], 0);
						dup2(pfd[i][1], 1);
						close(pfd[i-1][0]); close(pfd[i-1][1]);
						close(pfd[i][0]); close(pfd[i][1]);
					}

					// fprintf(stderr, "execvp:%s\n", my_argv[pipe_locate[i]+1]);
					if (myexecve(my_argv[pipe_locate[i] + 1], my_argv +pipe_locate[i] + 1, envp) < 0) {
						fprintf(stderr, "improper input\n");
						exit(1);
					}

					struct sigaction sa_sigabrt;
					memset(&sa_sigabrt, 0, sizeof sa_sigabrt);
					sa_sigabrt.sa_sigaction = abrt_handler;
					sa_sigabrt.sa_flags = SA_NOCLDWAIT;
					if (sigaction(SIGINT, &sa_sigabrt, NULL) < 0) {
						exit(1);
					}
					// exit(0);

				} //以下 parents
				else {
					bg_done_flg = 0;
					if (bgflg == 1) {
						bg_pid[bg_pid_cnt++] = pid;
						tcsetpgrp(STDOUT_FILENO, parent_pgid);
						tcsetpgrp(STDIN_FILENO, parent_pgid);
						bg_done_flg = 1;
						continue;
					}
					if (bg_done_flg == 1) {
						int pid = bg_pid[--bg_pid_cnt];
						int s;
						waitpid(pid, &s, 0);
					}

					if (i > 0) {
					   close(pfd[i-1][0]); close(pfd[i-1][1]);

				   } else if (i == 0) {

					   bg_done_flg = 0;
					   if (bgflg == 1) {
						   bg_pid[bg_pid_cnt++] = pid;
					   }
				   }
				}


			}


			int status;

			int a = pid;
			a = a - pipe_count;
			for (i = 0; i < pipe_count + 1 && pipe_count != 0; i++) {
				waitpid(a + i, &status, 0);
			}


		}

		if (pipe_count == 0) {

			// if pipe_cnt == 0 and dirflg == 1
			if ((cpid = fork()) < 0) {
				perror("fork");
				exit(1);
			}

			if (cpid == 0) {
				if (setpgid(getpid(), getpid()) < 0) {
					perror("setpgid");
					exit(1);
				}

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
								fd = open(my_argv[i+1], O_WRONLY|O_APPEND|O_CREAT, 0644);

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


					if (myexecve(my_argv[0], my_argv, envp) < 0) {
						fprintf(stderr, "mysh: command not found: %s\n", my_argv[0]);
						exit(1);
					}



					// exit(0);
//				}
			} else {
				// parent process
				// pid: process ID
				/*
				fprintf(stderr, "\t** parent: wait for child(%d) **\n", pid);*/
				// fprintf(stderr, "parent:%d\n", getpid());

				struct sigaction sa_sigabrt;
				memset(&sa_sigabrt, 0, sizeof sa_sigabrt);
				sa_sigabrt.sa_sigaction = abrt_handler;
				sa_sigabrt.sa_flags = SA_NOCLDWAIT;

				sigaction(SIGINT, &sa_sigabrt, NULL);

				if (bgflg == 1) {
					bg_pid[bg_pid_cnt++] = cpid;
				}

				if (bg_done_flg == 1) {
					int pid = bg_pid[--bg_pid_cnt];
					int s;
					waitpid(pid, &s, 0);
				}
				bg_done_flg = 0;

				if (bgflg == 1) {
					tcsetpgrp(STDOUT_FILENO, parent_pgid);
					tcsetpgrp(STDIN_FILENO, parent_pgid);
					bg_done_flg = 1;
					continue;
				}

				if (waitpid(cpid, &stat, 0) < 0) {

					 perror("wait");

					 // exit(1);
				}
				// if (wait(&stat) < 0) {
				// 	perror("wait");
				// }
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
