.PHONY: dbg clean


tkn: gettoken.c getargs.c
	gcc -o tkn gettoken.c getargs.c
dbg:
	gcc -ggdb gettoken.c getargs.c -o tkn
	lldb tkn
clean:
	rm tkn
	rm -rf tkn.dSYM

