#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXCHAR 256
#define MAXCOM 10


void getargs(int *argc, char *argv[], char *buf)
{
	
	char c;
	int cnt = 0;
	*argc = 0;
	int charcnt = 0;
	int prev = 1;

	int i = 0;

	do{
		c = buf[i++];

		if (cnt++ == MAXCHAR-1) {
			break;
		}

		if (c == EOF) {
			printf("\n");
			exit(0);
		}

		if (*argc  > MAXCOM-2) {
			break;
		}
		
		if ('!'<= c && c <= '~') {
			argv[*argc][charcnt++] = c;
			prev = 0;
		} else if (prev != 1 && (c == ' ' || c == '\t' || c ==  '\n' || c == '\0')) {
			prev = 1;
			argv[*argc][charcnt] = '\0';
			charcnt = 0;
			*argc += 1;
		} 	
	} while (c !=  '\n' && c!= '\0');


	//argv[*argc][charcnt] = '\0';

	argv[*argc] = NULL;

	/*
    printf("inputed:%d\n", *argc);
	int i;
	for (i = 0; i < *argc; i++) {
		printf("argv[%d]: ", i);
		printf("%s\n", argv[i]);
	} */
}


/*
int main()
{
    
    int my_argc;
    char **my_argv;
    
    //initialization
    my_argc = 0;
	my_argv = malloc(sizeof(char*) * MAXCOM);
	int i;
	for (i = 0; i < MAXCOM; i++) {
		my_argv[i] = malloc(sizeof(char) * MAXCHAR);
	}
    
	printf("input a line\n");
	char buf[MAXCHAR] = {};

	FILE *fp = stdin;
	if (fgets(buf, MAXCHAR-1, fp) == NULL) {
		if (ferror(fp)) {
			fprintf(stderr, "input error in fgets()\n");
			return -1;
		} else
			return 0;
	}

    getargs(&my_argc, my_argv, buf);

    for (i = 0; i < my_argc; i++) {
        printf("argv[%d]: %s\n", i, my_argv[i]);
		printf("sizeof argv: %lu\n", sizeof(my_argv[i])/(sizeof(my_argv[i][0])));
    }
    

    return 0;
}
*/
