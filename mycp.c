#include <stdio.h>

int main(int argc, char *argv[])
{
    FILE *fp = fopen(argv[1], "r");
    char buf[5];
    fread(buf, 1, sizeof(buf), fp);

    FILE *fp_w = fopen(argv[2], "a");
    fwrite(buf, 1, sizeof(buf), fp_w);

    return 0;
}
