.PHONY: dbg clean

simple: simple.c getargs.c
	gcc -o simple simple.c getargs.c
tkn: gettoken.c getargs.c
	gcc -o tkn gettoken.c getargs.c
dbg:
	gcc -ggdb simple.c getargs.c
	lldb a.out
clean:
	rm a.out
	rm -rf a.out.dSYM

