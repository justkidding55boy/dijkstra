#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include <ctype.h>
#define TOKENLEN    80

struct tkntype {
    int type;
    char *name;
} tkn[] = {
    {TKN_NORMAL,         "NORMAL"},
    {TKN_REDIR_IN,       "REDIR_IN"},
    {TKN_REDIR_OUR,      "REDIR_OUT"},
    {TKN_REDIR_APPEND,   "REDIR_APPEND"},
    {TKN_PIPE,           "PIPE"},
    {TKN_BG,             "BG"},
    {TKN_EOL,            "EOL"},
    {TKN_EOF,            "EOF"},
	{0,                  NULL}
};



char *pr_ttype(int);

int gettoken(char *token, int len, char*buf)
{
    int c, i = 0;
    char *p = token;

    *p = '\0';
  //  while(isblank(c = getchar()));

	int bc = 0;
	do {
		c = buf[bc++];
	} while(isblank(c));


    // if special character, return the corresponding type
    switch(c) {
        case EOF:
            return TKN_EOF;
        case '\n':
            return TKN_EOL;
		case '\0':
			return TKN_EOL;
        case '&':
            return TKN_BG;
        case '|':
            return TKN_PIPE;
        case '<':
            return TKN_REDIR_IN;
        case '>':
            if ((c = buf[bc++]) == '>')
                return TKN_REDIR_APPEND;
       //     ungetc(c, stdin);
			bc--;
            return TKN_REDIR_OUT;
        default:
            break;
	} 

    // nomal caracter is input

    //ungetc(c, stdin);
	bc--;
    for (i = 0; i < len - 1; i++) {
        c = buf[bc++];
        if (c != EOF && c != '\n' && c != '&' && c != '<' && c != '>' && c != '|' && !isblank(c))
            *p++ = c;
        else break;
    }
	bc--;
    *p = '\0';
    if (i > 0)
        return TKN_NORMAL;
    else
        return TKN_NONE;
}


/*
int main()
{
    char token[TOKENLEN];
    int ttype;

    for (;;) {
        fprintf(stderr, "input: ");
		char buf[MAXCHAR];
		fgets(buf, MAXCHAR-1, stdin);
        while (1) {
			ttype = gettoken(token, TOKENLEN, "\n");
			printf("%s", pr_ttype(ttype));
	
			if (ttype == TKN_EOL || ttype == TKN_EOF) {
				break;
			}

            printf("\tType: %s, \"%s\"\n", pr_ttype(ttype), token);
        }
        if (ttype == TKN_EOL)
            printf("\tType: %s, \"%s\"\n", pr_ttype(ttype), token);
		else
			break;
    }
	putchar('\n');
	return 0;
}*/


char *pr_ttype(int ttype)
{
	switch (ttype) {
		case TKN_NORMAL:
			return "NORMAL";
			break;
		case TKN_REDIR_IN:
			return "REDIR_IN";
			break;
		case TKN_REDIR_OUR:
			return "REDIR_OUT";
			break;
		case TKN_REDIR_APPEND:
			return "REDIR_APPEND";
			break;
		case TKN_PIPE:
			return "PIPE";
			break;
		case TKN_BG:
			return "BG";
			break;
		case TKN_EOL:
			return "EOL";
			break;
		case TKN_EOF:
			return "EOF";
			break;
		case TKN_NONE:
			return "NONE";
			break;
		case TKN_REDIR_OUT:
			return "REDIR_OUT";
			break;
		default:
			return "";
			break;
	}
}
