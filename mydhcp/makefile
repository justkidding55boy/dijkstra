.PHONY: clean
mydhcpc: socket.c proc.c mydhcp.h mydhcpd
	gcc -o mydhcpc socket.c proc.c
mydhcpd: server.c getargs.c proc.c mydhcp.h
	gcc -o mydhcpd server.c getargs.c proc.c

clean:
	rm mydhcpc mydhcpd
