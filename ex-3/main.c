#include <stdio.h>

int main()
{
	char c;
	while ((c = getchar()) != EOF) {
		ungetc(c, stdin);


	}
	return 0;
}
