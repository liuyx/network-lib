#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 1024

#define PATH "/tmp/unix-domain"
int main(int argc, char **argv) {
	int		sockfd;
	socklen_t len;

	struct sockaddr_un cliaddr;
	bzero(&cliaddr, sizeof(cliaddr));
	cliaddr.sun_family = AF_LOCAL;
	memcpy(cliaddr.sun_path,PATH, sizeof(PATH));

	if ( (sockfd = socket(AF_LOCAL,SOCK_STREAM,0)) < 0) {
		perror("socket");
		exit(1);
	}

	len = sizeof(cliaddr);
	connect(sockfd, (struct sockaddr *)&cliaddr,len);
	int 	n;
	char	sendline[MAXLINE], recvline[MAXLINE];
		
	while (fgets(sendline, MAXLINE, stdin) != NULL) {
		write(sockfd, sendline, strlen(sendline));

		n = read(sockfd, recvline, MAXLINE);
		recvline[n] = 0;
		fputs(recvline, stdout);
	}

	exit(0);
}
