#include "buf.h"


int findPATH(char **envp)
{
	int i;
	for (i = 0; envp[i] != NULL; i++) {
		if (strncmp(envp[i], "PATH", 4) == 0) {
			return i;
		}
	}

	fprintf(stderr, "I couldn't find PATH\n");
	return -1;
}
extern int getfile(char*, char *, char *);
int myexecve(char *file, char *argv[], char *ep[])
{

	int t;
	if ((t = findPATH(ep)) < 0) {
		return -1;
	}
	char pathall[MAX_BUF];
	char path[MAXPATH][MAX_BUF];
	//pl: path line
	int pl = 0;
	strncpy(pathall, ep[t]+5, MAX_BUF-1);
	int i, j = 0;
	for (i = 0; ; i++) {
		int c = pathall[i];
		if (c == ':' || c == '\0') {
			path[pl][j] = '\0';
			pl++;
			j = 0;
			if (c == '\0') break;
		} else {
			path[pl][j++] = c;
		}
	}

	char *files;
	files = malloc(sizeof (char) * MAXCHAR);
	memset(files, 0, MAXCHAR);
	int eflg = 0;
	for (i = 0; i < pl; i++) {
//		printf("path[%d]:%s\n", i, path[i]);
		char pt[MAX_BUF];
		sprintf(pt, "%s/", path[i]);
		char *ans;
		ans = malloc(sizeof (char) * MAXCHAR);
		memset(ans, 0, MAXCHAR);
		if (getfile(ans, file, pt) > 0) {
		//	printf("files:%s\n", files);
			eflg = 0;
			if (execve(ans, argv, ep) < 0 ) {
				perror("execve");
				return -1;
			} else {
				eflg = 0;
				return 1;
			}
		} else {
			eflg = 1;
		}
	}
	/*
	if (eflg == 1)
		fprintf(stderr, "command not found: %s\n", file);*/
		


	return -1;
}
/*
int main(int argc, char **argv, char *envp[])
{
	char **ep;
	int i;
	for (i = 0; envp[i] != NULL; i++) ;
	ep = malloc(sizeof (char *) * i);

	for (i = 0; envp[i] != NULL; i++) {
		ep[i] = malloc(sizeof (char) * MAX_BUF);
		memset(ep[i], 0, BUFSIZE);
		strncpy(ep[i], envp[i], MAX_BUF-1);
	}
	ep[i] = NULL;
	char *ch[] = {"ls", NULL};
	myexecve("ls", ch, ep);
	//execve("/bin/ls", ch, envp);

	return 0;
}
*/
