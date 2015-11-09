CC = gcc
time-client:
	$(CC) C2C/client.c tools.c -o out/client

time-server:
	$(CC) C2C/server.c tools.c -o out/server

server2:
	$(CC) C2C/server2.c tools.c -o out/server2

man:
	$(CC) C2C/man.c tools.c -o out/man

woman:
	$(CC) C2C/woman.c tools.c -o out/woman
