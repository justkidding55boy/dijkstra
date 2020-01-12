.PHONY: clean
myftpc: myftpc.c myftpd my.h
	gcc -Wall -o myftpc myftpc.c proc.c 
myftpd: myftpd.c my.h
	gcc -Wall -o myftpd myftpd.c proc.c

clean: 
	rm myftpc myftpd
