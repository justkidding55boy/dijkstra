#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXCHAR 256
#define MAXCOM 10


void getargs(int *argc, char *argv[])
{
    printf("$");
	
	char c;
	int cnt = 0;
	*argc = 0;
	int charcnt = 0;
	int prev = 1;

	do{
		c = getchar();

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
		} else if (prev != 1 && (c == ' ' || c == '\t' || c ==  '\n')) {
			prev = 1;
			argv[*argc][charcnt] = '\0';
			charcnt = 0;
			*argc += 1;
		} 		
	} while (c !=  '\n');

	
	argv[*argc][charcnt] = '\0';

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
*/
