#include <stdio.h>
#include <stdlib.h>
#include "mysh.h"
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


int gettoken(char *token, int len)
{
    int c, i = 0, type = TKN_NONE;
    char *p = token;

    *p = '\0';
    while(isblank(c = getchar()));

    // if special character, return the corresponding type
    switch(c) {
        case EOF:
            return TKN_EOF;
        case '\n':
            return TKN_EOL;
        case '&':
            return TKN_BG;
        case '|':
            return TKN_PIPE;
        case '<':
            return TKN_REDIR_IN;
        case '>':
            if ((c = getchar()) == '>')
                return TKN_REDIR_APPEND;
            ungetc(c, stdin);
            return TKN_REDIR_OUT;
        default:
            break;
    }

    // nomal caracter is input

    ungetc(c, stdin);
    for (i = 0; i < len - 1; i++) {
        c = getchar();
        if (c != EOF && c != '\n' && c != '&' && c != '<' && c != '>' && c != '|' && !isblank(c))
            *p++ = c;
        else break;
    }
    ungetc(c, stdin);
    *p = '\0';
    if (i > 0)
        return TKN_NORMAL;
    else
        return TKN_NONE;
}

int main()
{
    char token[TOKENLEN];
    int ttype;

    for (;;) {
        fprintf(stderr, "input: ");
        while (((ttype = gettoken(token, TOKENLEN)) != TKN_EOL && ttype != TKN_EOF)) {

            printf("\tType: %s, \"%s\"\n", pr_ttype(ttype), token);
        }
        if (ttype == TKN_EOL)
            printf("\tType: %s, \"%s\"\n", pr_ttype(ttype), token);
		else
			break;
    }
	putchar('\n');
	return 0;
}

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
