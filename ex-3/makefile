.PHONY: dbg clean

mysh: main.c input.c
	gcc -o mysh main.c getargs.c proc.c

dbg:
	gcc -ggdb main.c getargs.c proc.c
	lldb a.out


clean:
	rm a.out
	rm -rf a.out.dSYM
