#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main (int c, char *argv[]) {

    for (;;){
        fputs("test\n", stdout);
        //0 is file descriptor associated with the streams stdin.
        printf("stdin is %d\n", (int)tcgetpgrp(0));
        //1 is file descriptor associated with the streams stdout.
        printf("stdout is %d\n", (int)tcgetpgrp(1));
        sleep(5);
    }   

    return EXIT_SUCCESS;
}
