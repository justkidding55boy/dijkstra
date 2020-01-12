.PHONY: clean
myftpc: myftpc.c getargs.c proc.c myftpd my.h
	gcc -Wall -o myftpc myftpc.c proc.c getargs.c
myftpd: myftpd.c proc.c my.h
	gcc -Wall -o myftpd myftpd.c proc.c

clean: 
	rm myftpc myftpd
