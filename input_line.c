#include <stdio.h>
#define MAXCHAR 256

void getargs(int *argc, char *argv[])
{
    printf("input a line:");
    char line[MAXCHAR];
    fgets(line, MAXCHAR-1, stdin);
    printf("%s", line);

}

int main()
{
    
    int *my_argc;
    char my_argv[][MAXCHAR];
    
    //initialization
    *my_argc = 0;

    

    getargs(my_argc, my_argv);

    /*
    int i;
    for (i = 0; i < *my_argc; i++) {
        printf("argv[%d]: %s\n", i, my_argv[i]);
    }
    */

    return 0;
}

