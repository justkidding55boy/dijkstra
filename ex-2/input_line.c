#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXCHAR 256
#define MAXCOM 10


void getargs(int *argc, char *argv[])
{
    printf("input a line:");
	
	char c;
	int cnt = 0;
	*argc = 0;
	int charcnt = 0;
	int prev = 0;
	int err = 0;
	do{
		c = getchar();

		if (cnt++ == MAXCHAR-1) {
			break;
		}
		
		if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') ) {
			argv[*argc][charcnt++] = c;
			prev = 0;
		} else if (prev != 1 && (c == ' ' || c == '\t' || c ==  '\n')) {
			prev = 1;
			charcnt = 0;
			*argc += 1;
		} else {
			err = 1;
		} 
	} while (c !=  '\n');
	
	if (err == 1) {
		printf("The command should be words! (not numbers)\n");
	}
	
	argv[*argc][charcnt] = '\0';

	/*
    printf("inputed:%d\n", *argc);
	int i;
	for (i = 0; i < *argc; i++) {
		printf("argv[%d]: ", i);
		printf("%s\n", argv[i]);
	} */
}

int main()
{
    
    int my_argc;
    char **my_argv;
    
    //initialization
    my_argc = 0;
	int i;
	for (i = 0; i < MAXCOM; i++) {
		my_argv[i] = malloc(sizeof(char) * MAXCHAR);
	}
    
    getargs(&my_argc, my_argv);

    for (i = 0; i < my_argc; i++) {
        printf("argv[%d]: %s\n", i, my_argv[i]);
    }
    

    return 0;
}

