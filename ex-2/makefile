.PHONY: dbg clean

bufcache: main.c getblk.c buf.h
	gcc -o bufcache main.c getblk.c init.c input_line.c proc.c -lm 

dbg:
	gcc -g main.c getblk.c init.c input_line.c proc.c
	lldb a.out

clean:
	rm a.out
	rm -rf a.out.dSYM
