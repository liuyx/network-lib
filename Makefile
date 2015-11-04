CC = gcc
time-client:
	$(CC) C2C/client.c tools.c -o out/client

time-server:
	$(CC) C2C/server.c tools.c -o out/server

man:
	$(CC) C2C/man.c tools.c -o out/man

woman:
	$(CC) C2C/woman.c tools.c -o out/woman
