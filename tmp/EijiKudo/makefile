.PHONY: dbg clean

mysh: main.c getargs.c gettoken.c ls.c getfile.c
	gcc -o mysh main.c getargs.c proc.c gettoken.c ls.c getfile.c 

dbg:
	gcc -ggdb main.c getargs.c proc.c gettoken.c 001myls.c
	lldb a.out


clean:
	rm a.out
	rm -rf a.out.dSYM
