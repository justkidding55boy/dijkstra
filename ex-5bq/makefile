.PHONY: clean
myftpc: myftpc.c getargs.c proc.c myftpd my.h myls.c
	gcc -Wall -o myftpc myftpc.c proc.c getargs.c myls.c
myftpd: myftpd.c proc.c my.h myls.c
	gcc -Wall -o myftpd myftpd.c proc.c myls.c

clean: 
	rm myftpc myftpd
