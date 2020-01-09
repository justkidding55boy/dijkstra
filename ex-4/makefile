.PHONY: clean all
socket: socket.c
	gcc -o socket socket.c proc.c
server: server.c
	gcc -o server server.c getargs.c proc.c
all:
	gcc -o socket socket.c proc.c
	gcc -o test_socket test_socket.c proc.c
	gcc -o server server.c getargs.c proc.c

clean:
	rm socket server
