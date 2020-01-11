.PHONY: clean
myftpc: myftpc.c myftpd
	gcc -Wall -o myftpc myftpc.c 
myftpd: myftpd.c 
	gcc -Wall -o myftpd myftpd.c

clean: 
	rm myftpc myftpd
