.PHONY: dbg clean

mysh: main.c getargs.c gettoken.c
	gcc -o mysh main.c getargs.c proc.c gettoken.c

dbg:
	gcc -ggdb main.c getargs.c proc.c gettoken.c
	lldb a.out


clean:
	rm a.out
	rm -rf a.out.dSYM
