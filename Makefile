CC = gcc
CFLAGS = -O2 -g -o 
time-client:
	$(CC) C2C/client.c tools.c $(CFLAGS) out/client

time-server:
	$(CC) C2C/server.c tools.c $(CFLAGS) out/server

server2:
	$(CC) C2C/server2.c tools.c $(CFLAGS) out/server2

man:
	$(CC) C2C/man.c tools.c $(CFLAGS) out/man

woman:
	$(CC) C2C/woman.c tools.c $(CFLAGS) out/woman

udp-server:
	$(CC) C2C/udp-server.c tools.c $(CFLAGS) out/udp-server

udp-client:
	$(CC) C2C/udp-client.c tools.c $(CFLAGS) out/udp-server

us:
	$(CC) C2C/us.c tools.c $(CFLAGS) out/us

uc:
	$(CC) C2C/uc.c tools.c $(CFLAGS) out/uc
